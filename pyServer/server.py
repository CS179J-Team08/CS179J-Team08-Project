import boto3
import sys
import socket
import time
import json
from receivePacket import await_SQS_response
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
    time.sleep(8)
    s.close()
    if not data: 
        print("Error receiving client data")
        sys.exit()
    else:
        conn.sendall(bytes(str(length), 'utf-8'))
        time.sleep(5)
        conn.sendall(vaild_connection)
        print ("Message sent to the client")
    return conn

#Similar to the accept connection method we wait until a response from the c++ program requesting a file for playback. We still use a similar method of receiving and sending responses as done in accept_connection
def socket_server_await_request(conn):
    response = bytes("Getting a file for playback", 'utf-8')
    print("Awaitng next request")
    d = conn.recv(1024)
    data = d.decode()
    length = len(response.decode())
    print("The current length of response: " + str(length))
    print(data)
    time.sleep(5)
    if not data: 
        print("Error receiving client data")
        sys.exit()
    else:
        conn.sendall(bytes(str(length), 'utf-8'))
        time.sleep(5)
        conn.sendall(bytes("Getting a file for playback", 'utf-8'))
        print("Request acknowledged beginning file download")


#This method is the response sent back to the c++ that initially requested a file for playback. It takes in the current connection and the fileName to send.
def socket_server_respond_request(conn, fileName):
    reply = bytes(str(fileName), 'utf-8')
    length = len(reply.decode())
    print("Length of queue message: " + str(length))
    time.sleep(5)
    conn.sendall(bytes(str(length), 'utf-8'))
    time.sleep(5)
    conn.sendall(reply)

#The list_AWS_buckets method uses a boto3 client to print a list of the current bucket that exist on the S3 cloud service.
def list_AWS_buckets():
    #Initialze the s3 client
    s3 = boto3.client('s3')
    #The list_buckets method returns a dict of the buckets that exist in S3
    response = s3.list_buckets()
    #Begin to iterate over the dict and buckets and print out the name of each bucket
    print('Existing Buckets:')
    for bucket in response['Buckets']:
        #print(f'  {bucket["Name"]}')
        name = "{}".format(bucket["Name"])
        print(name)

#The aws_download method takes in a bucketName to look at, a fileName that represent a file path to search for in the given bucket, and storageResult as the user specified file name for the resulting download to have.

#Returns the storageResult string name
def aws_download(bucketName, fileName, storageResult):
    #Initialize a boto3 resource
    s3 = boto3.resource('s3')
    print(fileName)
    #Begin to download the specified fileName
    path = "../AudioEngine/" + storageResult
    s3.meta.client.download_file(bucketName, fileName, path)
    print(storageResult,"was downloaded successfully!")
    return storageResult


#Request_data returns a tuple of the fileName as a string, and the storageResult for what to name the downloaded file
def request_data():
    fileName = input("Enter the file to download: ")
    storageResult = input("What do you want to name the downloaded file: ")
    return (fileName, storageResult)


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


#bucket_menu runs the input and output needed for file downloads
#Returns a fileName of the file that was downloaded named by the user.
def bucket_menu():
    s3 = boto3.resource('s3')
    list_AWS_buckets()
    bucket_path = input("Which bucket do you want to look into?: ")
    target_bucket = s3.Bucket(bucket_path)
    for obj in target_bucket.objects.all():
        print(obj)
    target_data = request_data()
    fileName = confirm_file_is_vaild(bucket_path, target_bucket, target_data)
    return fileName


#Main function
if __name__ == '__main__':
    #Initialize our socket and connection to None, then begin their initialization
    s = None
    conn = None
    fileName = None
    s = socket_server_init()
    conn = socket_server_accept_connection(s)
    server_client =  boto3.resource('s3')
    bucket = "testing-pi"
    bucket_obj = server_client.Bucket(bucket)
    while 1:
        try:
           # socket_server_await_request(conn)
            #Once acknowledged, we can begin our file download
           # fileName = bucket_menu()
            #Now that we have a file for playback, send the name of the file over the socket connection
            sqs_response = await_SQS_response()
            confirm_file_is_vaild(bucket, bucket_obj, sqs_response)
            socket_server_respond_request(conn, sqs_response[1])
   # sys.exit()

#This can shutdown gracefully if you press control + c.
        except KeyboardInterrupt:
            s.close()
