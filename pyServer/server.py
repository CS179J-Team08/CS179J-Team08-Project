import boto3
import pickle
import sys
import socket
import time
import json
import os.path
from os import path
from receivePacket import await_SQS_response
from receivePacket import sqs_init
from receivePacket import sqs_register_device
from receivePacket import listen_for_registration

#Definitions of the HOST and PORT to use for the socket connection
HOST = 'localhost'	# Symbolic name meaning all available interfaces
PORT = 12345	# Arbitrary non-privileged port


#Author: Angel Renteria
#Output:A socket that will be used to create a server connection
#Input: None
#
#Description: This method begins the initialization of a socket that creates a TCP connection between itself and our C++ audio engine

def socket_server_init():
    try :
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print ('Socket created')
        return s
    except socket.error as msg :
        print ('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()


#Author: Angel Renteria
#Output: A connection object between this server and a c++ client
#
#Input: A socket connection representing our server connection
#
#Description: This method acts an acknowledgement between the server and client that a connection has been made. The server binds and begins to accept connections. Once this is done a series of receivess and sends are executed over the connection to ensure can be sent back and forth

def socket_server_accept_connection(s):
    #The response message to be sent back. The response must be sent as of data type bytes.
    #Here the bytes method formats the first arguement into the form of b'...' with the utf-8 encoding
    vaild_connection = bytes("Client connected to the server", 'utf-8')
    try:
        #Use the given HOST and PORT to bind to file descriptor
        s.bind((HOST, PORT))
        #Listen to the file descriptor for a client connection
        s.listen(5)
        print("waiting for client")
        #Here be use the accept method to confirm the connection with a client
        conn, addr = s.accept()
        print ("Connected by" + str(addr))
    except socket.error as msg:
        print ('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()

    #Here we specify that we want to receive data from our cliemt connection
    #The recv call is a blocking call and will hang unless a response is given.
    #Reading 1024 bytes worth of information from our client. It might not need to be this big
    print ('Socket bind complete')
    d = conn.recv(1024)
    #The information from our client is also in a byte format (b'...'). It must be decoded as shown below
    data = d.decode()

    #The following is the current manner in which data is sent back and forth. Some of these statements
    #are not really necessary, however this is to illustrate the back and forth communication between processes
    #We begin by first sending the c++ program the size our response, then we send the actual response.
    #This was done to ensure that the c++ program can actually continue to send and receive data without the socket dying
    #We implement some delays to visualize the back and forth, though I am not sure if they are needed to ensure a properly working system.
    length = len(vaild_connection.decode())
    print (d)
    print("The current length of the vaild connection response " + str(length))
    s.close()
    if not data: 
        print("Error receiving client data")
        sys.exit()
    else:
        conn.sendall(bytes(str(length), 'utf-8'))
        time.sleep(0.001)
        conn.sendall(vaild_connection)
        print ("Message sent to the client")
    return conn


#Author: Angel Renteria
#Output: None
#
#Input: The connection object that handles the sending and receiving of data over our TCP connection, and a string representing the SQS message
#
#Description: This method sends the information gotten from our SQS queue and sends it to the C++ program.

def socket_server_respond_request(conn, data):
    reply = bytes(str(data), 'utf-8')
    length = len(reply.decode())
    print("Length of queue message: " + str(length))
    conn.sendall(bytes(str(length), 'utf-8'))
    time.sleep(0.001)
    conn.sendall(reply)


#Author: Angel Renteria
#Output: A string representing the downloaded file's name
#
#Input: A string representing an S3 bucket name, a string representing a file name, a string representing the resulting downloaded file name
#
#Description: This method utilizes the boto3 library to access our S3 buckets and begin to download an file given the input arguments.

def aws_download(bucketName, fileName, storageResult):
    #Initialize a boto3 resource
    s3 = boto3.resource('s3')
    #Begin to download the specified fileName
    storage_path = "../AudioEngine/audio/" + storageResult
    if path.exists(storage_path) == False:
        s3.meta.client.download_file(bucketName, fileName, storage_path)
        print(storageResult,"was downloaded successfully!")
        return storageResult
    else:
        print("The requested file already exists in the system")
        return "file already exists"




#Author: Angel Renteria
#Output: String data that either represents a file that was downloaded or "file not found if the file does not exist in the S3 bucket
#
#Input: A string representing an S3 bucket name, a bucket object of the bucket to look into, and a tuple that contains the file name to search for as the first element
#
#Description: This method iterates over a buckets contents and begins to search if a given file name exists. If it does not exist then we return "file not found". Otherwise we begin our file download by calling aws_download()

def confirm_file_is_vaild(bucket_path, target_bucket, target_data):
    #Iterate over the object looking for the particular fileName in target_data. (fileName = target_data[0])
    for obj in target_bucket.objects.all():
        if target_data[0] in obj.key:
            #If the string of the fileName exists in the obj.key then we have found the file we want
            #We call aws_download and pass the bucket we want to download from, the exact path to the file for download, and the user specified file name for the given download
            fileName = aws_download(bucket_path, obj.key, target_data[0])
            return fileName
    print ("Requested file was not found. Awaiting for next queue message")
    return "file not found"

def find_user_queue():
    with open('device.json', 'r') as device:
        data = json.load(device)

    print(data)
    if data["queue"] == "empty":
        return (data, False)
    else:
        return (data, True)

#Main function
if __name__ == '__main__':
    #Initialize our socket and connection to None, then begin their initialization
    s = None
    conn = None
    fileName = None
    queue_id = None
    registration = find_user_queue()

    if registration[1] == False:
        print("This device is not connected to the web app")
        message_queue = sqs_register_device()
        while queue_id == None:
            queue_id = listen_for_registration(message_queue)
            print(queue_id)
        registration[0]["queue"] = queue_id
        new_json = { "device" : "", "queue" : ""}
        new_json["device"] = registration[0]["device"]
        new_json["queue"] = registration[0]["queue"]
        json_obj = json.dumps(new_json)
        with open('device.json', 'w') as json:
            json.write(json_obj)
        print("Device was registered")

    else:
        print("assigning the user queue to the specified value in device.json")
        queue_id = registration[0]["queue"]
        #assign queue value to python queue


    s = socket_server_init()
    conn = socket_server_accept_connection(s)
    server_client =  boto3.resource('s3')
    bucket = "cs-audiofile-bucketdefault-default"

    message_queue = sqs_init(queue_id)
    while 1:
        try:
            bucket_obj = server_client.Bucket(bucket)
            sqs_response = await_SQS_response(message_queue)
            if sqs_response[4] == True:
                if confirm_file_is_vaild(bucket, bucket_obj, sqs_response) is "file not found":
                    pass
                else:
                    socket_server_respond_request(conn, sqs_response[1])
            else:
                print("Error the received JSON from SQS is invaild")
                print("Waiting for the next request before sending data to the audio engine")

#This can shutdown gracefully if you press control + c.
        except KeyboardInterrupt:
            s.close()
