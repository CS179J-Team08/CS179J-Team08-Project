import pytest
import io
import sys
import os.path
import boto3
import threading
import socket
import time
from os import path
from receivePacket import mock_SQS_queue 
from receivePacket import verify_data
from server import socket_server_init
from server import socket_server_accept_connection
from server import confirm_file_is_vaild
from server import socket_server_respond_request
from server import aws_download

def test_initalization():
    s = None
    s = socket_server_init()
    assert s != None

def test_server_file_already_exists():
    s3 = boto3.resource('s3')
    bucketName = "testing-pi"
    fileName = "drumloop.wav"
    storageResult = fileName
    assert aws_download(bucketName, fileName, storageResult) == "file already exists"

def test_server_file_checker():
    s3 = boto3.resource('s3')
    bucketName = "testing-pi"
    fileName = "Igorrr- Downgrade Desert.flac"
    storageResult = fileName
    pathName = "../AudioEngine/audio/" + storageResult
    target_bucket = s3.Bucket(bucketName)
    target_data = (fileName, storageResult)
    confirm_file_is_vaild(bucketName, target_bucket, target_data)
    assert path.exists(pathName)

def test_server_fail_file_checker():
    s3 = boto3.resource('s3')
    bucketName = "testing-pi"
    fileName = "Happy Funtime.mp3"
    storageResult = "test.flac"
    target_bucket = s3.Bucket(bucketName)
    target_data = (fileName, storageResult)
    assert confirm_file_is_vaild(bucketName, target_bucket, target_data) == "file not found"


def test_server_connection():
    s = None
    s = socket_server_init()
    thread = threading.Thread(target=socket_server_accept_connection, args=(s,))
    thread.start()
    HOST = 'localhost'
    PORT = 12345
    datalen = None
    data = None
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    time.sleep(1)
    client_socket.connect((HOST, PORT))
    client_socket.sendall(bytes("Hello from pytest client", 'utf-8'))
    datalen = client_socket.recv(1024)
    data = client_socket.recv(1024)
    thread.join()
    client_socket.close()
    assert datalen != None
    assert data != None


def test_server_file_request_response():
    def test_server():
        server = socket_server_init()
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(('localhost', 12345))
        server.listen(1)
        conn, addr = server.accept()
        thread = threading.Thread(target=socket_server_respond_request, args=(conn, "testfile.txt"))
        thread.start()
        thread.join()
        server.close()

    thread = threading.Thread(target=test_server)
    thread.start()
    HOST = 'localhost'
    PORT = 12345
    datalen = None
    data = None
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    time.sleep(1)
    client_socket.connect((HOST, PORT))
    datalen = client_socket.recv(1024)
    data = client_socket.recv(1024)
    thread.join()
    client_socket.close()
    assert datalen != None
    assert data.decode() == "testfile.txt"

