import boto3
import json
import sys

#Author: Joseph DiCarlantonio
#Output: An SQS message queue
#Input: None
#
#Description: This method initializes the SQS queue used for receiving messages from an AWS SNS topic. By creating this we can receive messages from our cloud service on to the raspberry pi
maxQueueMessages = 10

def sqs_init():
    sqs = boto3.resource('sqs')
    sqsClient = boto3.client('sqs')

    #queue = sqs.get_queue_by_name(QueueName='PacketQueue')
    queueClient = sqsClient.create_queue(
            QueueName='6aa75722-15a4-488a-907a-e546c678d691'
            )
    queueUrl = queueClient['QueueUrl']
    queueAttr = sqsClient.get_queue_attributes(
                QueueUrl=queueUrl,
                AttributeNames=[
                    'QueueArn'
                ]
            )

    queueArn = queueAttr['Attributes']['QueueArn']

    queue = sqs.Queue(queueUrl)

    queuePolicy = {
        "Version": "2012-10-17", 
        "Id": queueArn + "/SQSDefaultPolicy",
        "Statement": [{
            "Sid": "Send_Receive_All",
            "Effect": "Allow",
            "Principal": "*",
            "Action": [
                "SQS:GetQueueAttributes",
                "SQS:GetQueueUrl",
                "SQS:ReceiveMessage",
                "SQS:SendMessage"
            ],
            "Resource": queueArn
        }]
    }

    queuePolicyStr = json.dumps(queuePolicy)

    response = queue.set_attributes(
                Attributes={
                    'Policy': queuePolicyStr     
                } 
            )
    print("Receiving messages from: {0}".format(queue.url))
    #print(queue.url)
    print('')
    return queue

#Author: Angel Renteria
#Output: A boolean value of True or False
#Input: A tuple containing a string file name, the playback option as a string, and lastly the string representation of the desired volume
#
#Description: This method is a used as a way to vailidate the message input coming in from the SQS queue. This is a first line of defense to ensure that our programs requiring this data can operate properly and as intended

def verify_data(queue_data):
    if queue_data[2].isnumeric() == False:
        return False
    elif queue_data[1] != "false" and queue_data[1] != "true":
        return False
    elif ',' in queue_data[0] or '{' in queue_data[0] or '}' in queue_data[0] or '\'' in queue_data[0] or ':' in queue_data[0] or "" is queue_data[0]:
        return False
    return True

#Author: Angel Renteria
#Output: A tuple containing five elements, the first a string representing a file name, the second being a string of the entire JSON message, third being a string denoting true or false for audio play/pause, fourth being a string representation of volume level, and lastly a boolean that denotes whether the input JSON data was verified to be vaild for use

#Input: A String representing JSON data
#
#Description: This method is a mock for testing purposes. Since we can not have the SQS queue send us messages when running tests we need to mimic the behavior that occurs when we receive a message on the device. We should extract key information and pass that data into verify_data() to error check

def mock_SQS_queue(string_data):
    fileName = None
    play_option = None
    volume_option = None
    message_to_engine = None
    data_is_vaild = None
    try:
        messageBody = json.loads(string_data)
        message_to_engine = messageBody
        print(messageBody)
        fileName = messageBody["filename"]
        play_option = messageBody["play"]
        volume_option = messageBody["parameters"]["volume"]
        print(fileName)
        print(play_option)
        print(volume_option)

        data_is_vaild = verify_data((fileName, play_option, volume_option))
        print (data_is_vaild)
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)
    except KeyError as error:
        print ("This given json does not have the following field " + str(error))
        data_is_vaild = False
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)

#Author: Angel Renteria
#Output: A tuple containing five elements, the first a string representing a file name, the second being a string of the entire JSON message, third being a string denoting true or false for audio play/pause, fourth being a string representation of volume level, and lastly a boolean that denotes whether the input JSON data was verified to be vaild for use
#
#Input: An SQS message queue
#
#Description: This method is the key point of communication between the raspberry pi and the AWS cloud. We wait on the SQS queue to be populated with messages that are then packaged as a JSON and begin to extract key elements from the JSON. We then verify the incoming message and return our tuple the waiting python server that made a call to this method.

def await_SQS_response(queue):
    waiting_for_SQS = 1
    fileName = None
    play_option = None
    volume_option = None
    message_to_engine = None
    data_is_vaild = None
    try:
        while waiting_for_SQS:
            # receive messages from SQS queue here
            for message in queue.receive_messages(MaxNumberOfMessages=maxQueueMessages):
                messageBody = json.loads(message.body)
                message_to_engine = messageBody
                #actualMessages.append(messageBody)
                print(messageBody)
                fileName = messageBody["filename"]
                play_option = messageBody["play"]
                volume_option = messageBody["parameters"]["volume"]
                print(fileName)
                print(play_option)
                print(volume_option)
                message.delete()
                waiting_for_SQS = 0

        data_is_vaild = verify_data((fileName, play_option, volume_option))
        print (data_is_vaild)
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)
    except KeyError as error:
        print ("This given json does not have the following field " + str(error))
        data_is_vaild = False
        queue.purge()
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)
    except KeyboardInterrupt:
        sys.exit()


#if __name__ == '__main__':
#   while 1:
#      try:
#    queue = sqs_init()
#    await_SQS_response(queue)
#
#This can shutdown gracefully if you press control + c.
#        except KeyboardInterrupt:
#            sys.exit()
