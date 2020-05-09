import boto3
import json
import sys

sqs = boto3.resource('sqs')
sqsClient = boto3.client('sqs')

#queue = sqs.get_queue_by_name(QueueName='PacketQueue')
queueClient = sqsClient.create_queue(
        QueueName='23292c32-66b1-409a-9dd4-e25322cba530'
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

maxQueueMessages = 10
actualMessages = []

print("Receiving messages from: {0}".format(queue.url))
#print(queue.url)
print('')

def verify_data(queue_data):
    if queue_data[2].isnumeric() == False:
        return False
    elif queue_data[1] != "false" and queue_data[1] != "true":
        return False
    elif "," is queue_data[1] or "{" is queue_data[0] or "}" is queue_data[0] or "'" is queue_data[0] or ":" is queue_data[0] or queue_data[0] is "":
        return False
    return True

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

        data_is_vaild = verify_data((fileNane, play_option, volume_option))
        print (data_is_vaild)
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)
    except KeyError as error:
        print ("This given json does not have the following field " + str(error))
        data_is_vaild = False
        return (fileName, message_to_engine, play_option, volume_option, data_is_vaild)

def await_SQS_response():
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
                volume_option = messageBody["parameters"][0]["volume"]
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



#if __name__ == '__main__':
#    while 1:
#        try:
#            await_SQS_response()
#
##This can shutdown gracefully if you press control + c.
#        except KeyboardInterrupt:
#            sys.exit()
