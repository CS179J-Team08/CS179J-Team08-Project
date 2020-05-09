import boto3
import sys
import socket
import time
import json
from receivePacket import await_SQS_response
from receivePacket import sqs_init
#Definitions of the HOST and PORT to use for the socket connection
HOST = 'localhost'	# Symbolic name meaning all available interfaces
PORT = 12345	# Arbitrary non-privileged port


#This method begins the socket creation. We are using a TCP connection given the SOCK_STREAM option
def socket_server_init():
    try :
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        print ('Socket created')
        return s
    except socket.error as msg :
        print ('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()


#This method initiates the creation of the python server.
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
        conn.sendall(vaild_connection)
        print ("Message sent to the client")
    return conn


#This method is the response sent back to the c++ that initially requested a file for playback. It takes in the current connection and the fileName to send.
def socket_server_respond_request(conn, fileName):
    reply = bytes(str(fileName), 'utf-8')
    length = len(reply.decode())
    print("Length of queue message: " + str(length))
  #  time.sleep(5)
    conn.sendall(bytes(str(length), 'utf-8'))
   # time.sleep(5)
    conn.sendall(reply)


#Returns the storageResult string name
def aws_download(bucketName, fileName, storageResult):
    #Initialize a boto3 resource
    s3 = boto3.resource('s3')
    print(fileName)
    #Begin to download the specified fileName
    download_path = "publc/" + fileName
    path = "../AudioEngine/audio/" + storageResult
    s3.meta.client.download_file(bucketName, fileName, path)
    print(storageResult,"was downloaded successfully!")
    return storageResult




#confirm_file_is_vaild is method that takes in a bucket to target, the bucket as an object to iterate over, and a tuple called target_data that represents the fileName, and storageResult we want from a given bucket.

#Returns a string called fileName that contains the user specified file name.
def confirm_file_is_vaild(bucket_path, target_bucket, target_data):
    #Iterate over the object looking for the particular fileName in target_data. (fileName = target_data[0])
    for obj in target_bucket.objects.all():
        if target_data[0] in obj.key:
            #If the string of the fileName exists in the obj.key then we have found the file we want
            #We call aws_download and pass the bucket we want to download from, the exact path to the file for download, and the user specified file name for the given download
            fileName = aws_download(bucket_path, obj.key, target_data[0])
            return fileName
    print ("file not found")
    return "file not found"



#Main function
if __name__ == '__main__':
    #Initialize our socket and connection to None, then begin their initialization
    s = None
    conn = None
    fileName = None
    s = socket_server_init()
    conn = socket_server_accept_connection(s)
    server_client =  boto3.resource('s3')
    bucket = "cs-audiofile-bucketdefault-default"
    message_queue = sqs_init()
    while 1:
        try:
            bucket_obj = server_client.Bucket(bucket)
            sqs_response = await_SQS_response(message_queue)
            if sqs_response[4] == True:
                confirm_file_is_vaild(bucket, bucket_obj, sqs_response)
                #socket_server_respond_request(conn, sqs_response[1])
                socket_server_respond_request(conn, sqs_response[0])
            else:
                print("Error the received JSON from SQS is invaild")
                print("Waiting for the next request before sending data to the audio engine")

#This can shutdown gracefully if you press control + c.
        except KeyboardInterrupt:
            s.close()
