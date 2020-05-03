import boto3
import sys
import socket

HOST = 'localhost'	# Symbolic name meaning all available interfaces
PORT = 8888	# Arbitrary non-privileged port

def socket_server_init():
# Datagram (udp) socket
    try :
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        print ('Socket created')
        return s
    except socket.error as msg :
        print ('Failed to create socket. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()



def socket_server_accept_connection(s):
# Bind socket to local host and port
    vaild_connection = bytes("Client connected to the server", 'utf-8')
    try:
        s.bind((HOST, PORT))
    except socket.error as msg:
        print ('Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1])
        sys.exit()
	
    print ('Socket bind complete')

    d = s.recvfrom(1024)
    data = d[0]
    addr = d[1]
    print (str(data))
    if not data: 
        print("Error receiving client data")
        s.close()
        sys.exit()
    else:
        #reply = vaild_connection
        s.sendto(vaild_connection, addr)
        print ('Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(data).strip())

def socket_server_await_request(s):
    print("Awaitng next request")
    d = s.recvfrom(1024)
    data = d[0]
    addr = d[1]
    if not data: 
        print("Error receiving client data")
        s.close()
        sys.exit()
    if str(data) is "Connection to server from AudioEngine":
        reply = vaild_connection
        s.sendto(reply, addr)
        print ('Message[' + addr[0] + ':' + str(addr[1]) + '] - ' + str(data).strip())
        return addr
    else:
        return -1

def socket_server_respond_request(s, addr, fileName):
    reply = bytes(fileName, 'utf-8')
    s.sendto(reply, addr)


def list_AWS_buckets():
    s3 = boto3.client('s3')
    response = s3.list_buckets()
    print('Existing Buckets:')
    for bucket in response['Buckets']:
        print(f'  {bucket["Name"]}')


def aws_download(bucketName, fileName, storageResult):
    s3 = boto3.resource('s3')
    print(fileName)
    s3.meta.client.download_file(bucketName, fileName, storageResult)
    print(storageResult,"was downloaded successfully!")
    return storageResult

def request_data():
    fileName = input("Enter the file to download: ")
    storageResult = input("What do you want to name the downloaded file: ")
    return (fileName, storageResult)

def confirm_file_is_vaild(bucket_path, target_bucket, target_data):
    for obj in target_bucket.objects.all():
        if target_data[0] in obj.key:
            fileName = aws_download(bucket_path, obj.key, target_data[1])
            return fileName
    print ("file not found")

def bucket_menu():
    s3 = boto3.resource('s3')
    bucket_path = input("Which bucket do you want to look into?: ")
    target_bucket = s3.Bucket(bucket_path)
    for obj in target_bucket.objects.all():
        print(obj)
    target_data = request_data()
    fileName = confirm_file_is_vaild(bucket_path, target_bucket, target_data)
    return fileName


if __name__ == '__main__':
    try:
        s = None
        s = socket_server_init()
        socket_server_accept_connection(s)
        s3 = boto3.resource('s3')
        while 1:
       #         list_AWS_buckets()
       #         fileName = bucket_menu()
       #         addr = socket_server_await_request(s)
       #         if addr is -1:
       #             print("Error could not resolve address of client")
       #         else:
       #             socket_server_respond_request(s, addr, fileName)
        s.close()
        sys.exit()

    except KeyboardInterrupt:
        s.close()
