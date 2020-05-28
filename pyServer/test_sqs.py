import pytest
import io
import sys
import os.path
import boto3
import threading
import socket
import time
import json
from os import path
from receivePacket import mock_SQS_queue 
from receivePacket import verify_data

def test_sqs_with_vaild_json():
    data = {
   "group": [
      {"userID": "TutorialTestQueue"},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""}
   ],
   "filename": "Igorr- Downgrade Desert.flac",
   "play": "false",
   "parameters":   {
      "volume":"0"
   }
}

    json_data = json.dumps(data)
    response = mock_SQS_queue(json_data)
    assert response[4] == True

def test_sqs_playback_options_match():
    data = {
   "group": [
      {"userID": "TutorialTestQueue"},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""}
   ],
   "filename": "Igorr- Downgrade Desert.flac",
   "play": "false",
   "parameters":   {
      "volume":"0.0"
   }
}

    json_data = json.dumps(data)
    response = mock_SQS_queue(json_data)
    assert response[4] == True
    assert response[3] == "0.0"
    assert response[2] == "false"
    assert response[0] == "Igorr- Downgrade Desert.flac"

def test_sqs_with_invaild_json():
    data = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],}
    data_1 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}], "play": "true","parameters":   {"volume":"0"}}
    data_2 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- 'Downgrade Desert.flac","parameters":   {"volume":"0"}}
    data_3 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- 'Downgrade Desert.flac","play": "true","parameters":   {}}
    data_4 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- 'Downgrade Desert.flac","play": "true"}

    json_data = json.dumps(data)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_1)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_2)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_3)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_4)
    response = mock_SQS_queue(json_data)
    assert response[4] == False

def test_sqs_with_play_option_invaild():
    data = {
   "group": [
      {"userID": "TutorialTestQueue"},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""}
   ],
   "filename": "Igorr- Downgrade Desert.flac",
   "play": "hello world",
   "parameters":   {
      "volume":"0"
   }
}
    json_data = json.dumps(data)
    response = mock_SQS_queue(json_data)
    assert response[4] == False

def test_sqs_with_volume_option_invaild():
    data = {
   "group": [
      {"userID": "TutorialTestQueue"},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""},
      {"userID": ""}
   ],
   "filename": "Igorr- Downgrade Desert.flac",
   "play": "true",
   "parameters":   {
      "volume":"1.1a"
   }
}
    json_data = json.dumps(data)
    response = mock_SQS_queue(json_data)
    assert response[4] == False

def test_sqs_with_filename_special_characters():
    data_1 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- 'Downgrade Desert.flac","play": "true","parameters":   {"volume":"0"}}
    data_2 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- {Downgrade Desert.flac","play": "true","parameters":   {"volume":"0"}}
    data_3 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- }Downgrade Desert.flac","play": "true","parameters":   {"volume":"0"}}
    data_4 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- ,Downgrade Desert.flac","play": "true","parameters":   {"volume":"0"}}
    data_5 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "","play": "true","parameters":   {"volume":"0"}}
    data_6 = {"group": [{"userID": "TutorialTestQueue"},{"userID": ""},{"userID": ""},{"userID": ""},{"userID": ""}],"filename": "Igorr- Downgrade: Desert.flac","play": "true","parameters":   {"volume":"0"}}
    json_data = json.dumps(data_1)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_2)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_3)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_4)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_5)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
    json_data = json.dumps(data_6)
    response = mock_SQS_queue(json_data)
    assert response[4] == False
