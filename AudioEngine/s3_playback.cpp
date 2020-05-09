#include "AudioEngine.h"
#include "PacketParser.h"

#include <cstring>
#include <string>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fstream>
#include "json.hpp"

using json = nlohmann::json;
//Arbitrary port number needed for connection to python program.
#define PORT 12345

/* Author: Angel Renteria
 * Name: server_client_init
 * Inputs: int pointer called sockfd specifying a file descriptor, struct sockaddr_in pointer describes the transport address and port we are using
 * Output: None
 *
 * Description: This method initializes the creation of a socket and begins its connection to a server. In this case it connects our c++ program to a python program that is running the server connection and listening for a connection.
 */
void server_client_init(int* sockfd, struct sockaddr_in* servaddr)  {
	if( (*(sockfd) = socket(AF_INET, SOCK_STREAM, 0)) < 0)  {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&(*(servaddr)), 0 , sizeof(*(servaddr)));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &(*(servaddr)).sin_addr);
	if(connect(*(sockfd), (struct sockaddr*)&(*(servaddr)), sizeof(*(servaddr))) < 0)   {
		printf("Connection Failed\n");
		exit(0);
	}
}


/* Author: Angel Renteria
 * Name: server_client_await_request
 * Inputs: int pointer called sockfd specifying a file descriptor, char pointer called dataBuffer that will store the incomming message from our python server, and struct sockaddr_in pointer called servaddr that describes the transport address and port we are using
 * Output: None
 *
 * Description: This method clears the dataBuffer and begins a two phase transaction between the client and server. First, the client begins with the read() blocking call that waits for a size of an incomming message. Second, once the size of the incomming message is defined we read from the stream the amount of bytes that were determined from the first call. This is to ensure that we don't exceed the size of the incomming stream since this does seem to cause and issue. If you don't send the enough data that was passed into read, we wait indefinitely until that amount of data has been received. Leading to undefined behaviour for both our c++ and python programs.
 */
void server_client_await_request(int sockfd, char* dataBuffer, struct sockaddr_in* servaddr)   {
	int n;	
	memset(dataBuffer, 0 , 1024);
	n = read(sockfd, dataBuffer, 100);
	if(n < 0)   {
		printf("Error in Socket Connection\n");
	}
	else   {
		printf("Current size of stream: %d\n", n);
		printf("Current Buffer: %s\n", dataBuffer - 1);
		n = atoi(dataBuffer);
		printf("Size of incoming stream: %d\n", n);
		read(sockfd, dataBuffer, n);
		dataBuffer[n] = '\0';
	}
	printf("\n");
}

/* Author: Angel Renteria
 * Name: sig_Handler
 * Inputs: int value called sockfd specifying a file descriptor.
 * Output: None
 *
 * Description: This method handles the control + c signal that is sent from the keyboard. When control + c is pressed, the handler interrupts the program and executes this function. Here we use this handler to gracefully shutdown our program using control + c. In the sense that we close the socket before the program is terminated.
 */
void sig_Handler(int sockfd)   {
	printf("Exiting execution with control-c\n");
	close(sockfd);
	exit(1);
}

string parse_json_for_filename(json audio_options)   {
	json::iterator iter = audio_options.begin();
	struct stat buffer;
	string fileName = iter.value();
	if(stat(fileName.c_str(), &buffer) == 0)   {
		return iter.value();
	}
	else   { 
		return "Does not exist";
	}
}

bool verify_json_exists(string fileName)   {
	struct stat buffer;
	if(stat(fileName.c_str(), &buffer) == 0)   {
		return true;
	}
	else   {
		return false;
	}
}

int main()
{
    //Initialization of variables
    int sockfd;
    int result;
    char dataBuffer[1024];
    char *client_ack = "Connection to server from AudioEngine";
    char *client_request = "Requesting a file for playback";
    char *dataPtr = dataBuffer;
    packetParser parser; // for parsing json string packets
    dataPacket audioSettings; // data packet with audio settings
    //Initialzation of our sigIntHandler. Here we assign our sigIntHandler the function to execute and to what specific signal it should catch. The SIGINT signal is what is sent when we press control + c on the keyboard
    struct sigaction sigIntHandler;
    sigIntHandler.sa_handler = sig_Handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, NULL);
    struct sockaddr_in servaddr;
    json audio_options;

    //Init the client
    server_client_init(&sockfd, &servaddr);

    //Send a response to acknowledge the connection from client to server
    printf("Sending Client Connection\n");
    result = sendto(sockfd, client_ack, strlen(client_ack), 0, NULL, 0);
    printf("Result: %d\n", result);
    printf("Message sent!\n");
//	sleep(5);

    //Wait for the server's response 
    server_client_await_request(sockfd, dataPtr, &servaddr);

    /*
    //Initiate a request for file playback by sending a request
    printf("Sending request for playback info\n");
    sleep(5);
    result = sendto(sockfd, client_request, strlen(client_request), 0, NULL, 0);
    printf("Result: %d\n", result);
    sleep(5);

    //Await the response of the playback request
    server_client_await_request(sockfd, dataPtr, &servaddr);
    printf("Waiting for file to playback\n");
    sleep(5);

    //Now wait for the file to play
    */
    printf("Listening to python server\n");
    while(1)   
    {
        server_client_await_request(sockfd, dataPtr, &servaddr);
//        printf("This is what is in the data buffer: %s\n", dataPtr);

        // start parser
        string jsonDataPacket(dataPtr);
	parser.parseData(jsonDataPacket);
        audioSettings = parser.getCurrentRequest();

        cout << "filename: " << audioSettings.filename << '\n';
        cout << "Play: " << audioSettings.play << '\n';
        cout << "Volume: " << audioSettings.volume << '\n';
    }

/*
    if(verify_json_exists(jsonFile))   {
            std::ifstream jsonStream(jsonFile);
            jsonStream >> audio_options;
    }
    else   {
            printf("Error the json given to the audio engine does not exist\nShutting down\n");
            exit(0);
    }

    //Simple file playback that used the main code found in AudioEngine.cpp
    //We just load the given file name from dataPtr into the audio engine and begin playback.
    string n = "test System";
    string m = parse_json_for_filename(audio_options);
    if(m == "Does not exist")   {
            printf("The file specified in the json does not exist. Shutting down\n");
            exit(0);
    }
    auto ae = new audioEngine();
    auto de = new dspEngine();
    ae->init();
    ae->addSystem(n);
    ae->loadSound(n, m, true, true, false);
    int id = ae->aePlaySound(n, m);
    ae->setChannelVolume(n, id, 0);
    //At this point we loop the track infinitely. Use control + c to shutdown the program gracefully.
    while (1) { ae->update(); }
    return 0; 
    */
}
