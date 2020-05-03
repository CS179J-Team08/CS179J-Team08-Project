#include "AudioEngine.h"
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#define PORT 8888


void server_client_init(int* sockfd, struct sockaddr_in* servaddr)  {
	if( (*(sockfd) = socket(AF_INET, SOCK_DGRAM, 0)) < 0)  {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&(*(servaddr)), 0 , sizeof(servaddr));
	servaddr->sin_family = AF_INET;
	servaddr->sin_port = htons(PORT);
	servaddr->sin_addr.s_addr = INADDR_ANY;
}


void server_client_await_request(int sockfd, char* dataBuffer, struct sockaddr_in* servaddr, socklen_t len)   {
	int n;
	n = recvfrom(sockfd, (char*)dataBuffer, 1024, MSG_WAITALL, (struct sockaddr *) &(*(servaddr)), &len);
	dataBuffer[1024] = '\0';
	printf("Server :%s\n", dataBuffer);
}

int main()
{
	int sockfd;
	char dataBuffer[1024];
	char *client_ack = "Connection to server from AudioEngine";
	char *dataPtr = dataBuffer;
	struct sockaddr_in servaddr;
	server_client_init(&sockfd, &servaddr);
	socklen_t len;
	sendto(sockfd, (const char*)client_ack, strlen(client_ack), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
	printf("Message sent!\n");
	server_client_await_request(sockfd, dataPtr, &servaddr, len);
	close(sockfd);
	
/*	string n = "test System";
	string m =   "audio/drumloop.wav";
	auto ae = new audioEngine();
	auto de = new dspEngine();
	ae->init();
	ae->addSystem(n);
	ae->loadSound(n, m, true, true, false);
	int id = ae->aePlaySound(n, m);
	ae->setChannelVolume(n, id, 0);
	while (1) { ae->update(); }
	return 0; */
}
