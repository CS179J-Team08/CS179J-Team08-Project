import pytest
import os.path
import boto3
import threading
import socket
import time
import json
from receivePacket import mock_SQS_queue 
from receivePacket import verify_data
from server import socket_server_init
from server import socket_server_accept_connection
from server import socket_server_respond_request
from server import message_url_check
from server import find_user_queue
from server import thread_send

def test_initalization():
    s = None
    s = socket_server_init()
    assert s != None

def test_server_url_generation():
    data = { 'group': [
        { 'userID': '' },
        { 'userID': '' },
        { "userID": '' },
        { "userID": '' },
        { 'userID': '' },
    ],
    'filenames': [
        { 'name': 'drumloop.wav' },
        { 'name': 'jaguar.wav'}
    ],
    'play': 'false',
    'stop': 'false',
    'parameters': {
      'volume': '0.0',
      'echo': {
        'apply': 'false',
        'delay': '10.0',
        'feedback': '0.0',
        'dry': '0.0',
        'wet': '0.0'
      },
      'equalizer': {
        'apply': 'false',
        'lowgain': '0.0',
        'midgain': '0.0',
        'highgain': '0.0'
      }
    }
}
    s3 = boto3.resource('s3')
    bucketName = "cs-audiofile-bucketdefault-default"
    target_bucket = s3.Bucket(bucketName)
    files = data["filenames"]
    sqs_response = (files, True)
    result = message_url_check(sqs_response, bucketName, target_bucket)
    print(result)
    assert result[0][0]["name"] != ""



def test_server_fail_url_checker():
    data = { "group": [
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
    ],
    "filenames": [
        { "name": "drumloop.wav" },
        { "name": "jaguar.wav"}
    ],
    "play": "false",
    "stop": "false",
    "parameters": {
      "volume": "0.0",
      "echo": {
        "apply": "false",
        "delay": "10.0",
        "feedback": "0.0",
        "dry": "0.0",
        "wet": "0.0"
      },
      "equalizer": {
        "apply": "false",
        "lowgain": "0.0",
        "midgain": "0.0",
        "highgain": "0.0"
      }
    }
}
    s3 = boto3.resource('s3')
    bucketName = "testing-pi"
    target_bucket = s3.Bucket(bucketName)
    files = data["filenames"]
    sqs_response = (files, True)
    result = message_url_check(sqs_response, bucketName, target_bucket)
    assert result[0][0]["name"] == ""

def test_file_does_not_exist():
    data = { "group": [
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
        { "userID": "" },
    ],
    "filenames": [
        { "name": "funny_song.wav" },
        { "name": "happy_fun_time.wav"}
    ],
    "play": "false",
    "stop": "false",
    "parameters": {
      "volume": "0.0",
      "echo": {
        "apply": "false",
        "delay": "10.0",
        "feedback": "0.0",
        "dry": "0.0",
        "wet": "0.0"
      },
      "equalizer": {
        "apply": "false",
        "lowgain": "0.0",
        "midgain": "0.0",
        "highgain": "0.0"
      }
    }
}
    s3 = boto3.resource('s3')
    bucketName = "testing-pi"
    target_bucket = s3.Bucket(bucketName)
    files = data["filenames"]
    sqs_response = (files, True)
    result = message_url_check(sqs_response, bucketName, target_bucket)
    for files in result[0]:
        assert files["name"] == ""

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

def test_find_device_queue():
    result = find_user_queue()
    assert result[0]["device"] == "device-#"
    assert result[0]["queue"] == "empty"
    assert result[1] == False

def test_thread_send():
    def test_server():
        server = socket_server_init()
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(('localhost', 12345))
        server.listen(1)
        conn, addr = server.accept()
        data = { "group": [
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
        ],
        "filenames": [
            { "name": "drumloop.wav" },
            { "name": "jaguar.wav"}
        ],
        "play": "false",
        "stop": "false",
        "parameters": {
          "volume": "0",
          "echo": {
            "apply": "false",
            "delay": "10.0",
            "feedback": "0.0",
            "dry": "0.0",
            "wet": "0.0"
          },
          "equalizer": {
            "apply": "false",
            "lowgain": "0.0",
            "midgain": "0.0",
            "highgain": "0.0"
           }
         }
        }
        s3 = boto3.resource('s3')
        bucketName = "cs-audiofile-bucketdefault-default"
        target_bucket = s3.Bucket(bucketName)
        files = data["filenames"]
        sqs_response = (files, data, None, None, True)
        thread_send(bucketName, target_bucket, sqs_response, conn)
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
    assert data.decode() != ""

def test_thread_fail_send():
    def test_server():
        server = socket_server_init()
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server.bind(('localhost', 12345))
        server.listen(1)
        conn, addr = server.accept()
        data = { "group": [
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
            { "userID": "" },
        ],
        "filenames": [
            { "name": "test.wav" },
            { "name": "fake_file.wav"}
        ],
        "play": "false",
        "stop": "false",
        "parameters": {
          "volume": "0",
          "echo": {
            "apply": "false",
            "delay": "10.0",
            "feedback": "0.0",
            "dry": "0.0",
            "wet": "0.0"
          },
          "equalizer": {
            "apply": "false",
            "lowgain": "0.0",
            "midgain": "0.0",
            "highgain": "0.0"
           }
         }
        }
        s3 = boto3.resource('s3')
        bucketName = "testing-pi"
        target_bucket = s3.Bucket(bucketName)
        files = data["filenames"]
        sqs_response = (files, data, None, None, False)
        thread_send(bucketName, target_bucket, sqs_response, conn)
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
    assert data.decode() != None

