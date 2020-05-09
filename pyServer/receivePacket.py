import boto3
import json

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

def await_SQS_response():
    waiting_for_SQS = 1
    fileName = None
    message_to_engine = None
    while waiting_for_SQS:
        # receive messages from SQS queue here
        for message in queue.receive_messages(MaxNumberOfMessages=maxQueueMessages):
            messageBody = json.loads(message.body)
            message_to_engine = messageBody
            #actualMessages.append(messageBody)
            print(messageBody)
            fileName = messageBody["filename"]
            message.delete()
            waiting_for_SQS = 0

    actualMessages.clear()
    return (fileName, message_to_engine)

#if __name__ == '__main__':
#    response = None
#    while 1:
#        response = await_SQS_response()
#        print(response[0])
#        print(response[1])
#        break
