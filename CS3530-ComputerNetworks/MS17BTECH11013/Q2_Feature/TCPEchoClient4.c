#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <time.h>

#define BUFSIZE 1024

int main(int argc, char **argv) {

	if (argc != 3) {
		perror("<Server Address> <Server Port> ");
		exit(-1);
	}

	char *servIP = argv[1];
	char *echoString;


	// Set port number as given by user or as default 12345
	// in_port_t servPort = (argc == 3) ? atoi(argv[2]) : 12345;

	// Set port number as user specifies
	in_port_t servPort = atoi(argv[2]);

	//Creat a socket
	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0) {
		perror("socket() failed");
		exit(-1);
	}

	// Set the server address
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	int err = inet_pton(AF_INET, servIP, &servAddr.sin_addr.s_addr);
	if (err <= 0) {
		perror("inet_pton() failed");
		exit(-1);
	}
	servAddr.sin_port = htons(servPort);

	// Connect to server
	if (connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0) {
		perror("connect() failed");
		exit(-1);
	}


	// Send string to server

    time_t current_time;
    char* c_time_string;

    /* Obtain current time. */
    current_time = time(NULL);

    echoString = ctime(&current_time);

    printf("Time Stamp when send from the client is %s\n", echoString);

    size_t echoStringLen = strlen(echoString);

	ssize_t sentLen = send(sockfd, echoString, echoStringLen, 0);
	if (sentLen < 0) {
		perror("send() failed");
		exit(-1);
	} else if (sentLen != echoStringLen) {
		perror("send(): sent unexpected number of bytes");
		exit(-1);
	}

	// Receive string from server
	unsigned int totalRecvLen = 0;

	fputs("Received: ", stdout);
	while (totalRecvLen < echoStringLen) {
		char buffer[BUFSIZE];
		memset(buffer, 0, BUFSIZE);
		ssize_t recvLen = recv(sockfd, buffer, BUFSIZE - 1, 0);
		if (recvLen < 0) {
			perror("recv() failed");
			exit(-1);
		} else if (recvLen == 0) {
			perror("recv() connection closed prematurely");
			exit(-1);
		}

		totalRecvLen += recvLen;
		buffer[recvLen] = '\n';

		fputs(buffer, stdout);
	}

	close(sockfd);
	exit(0);
}
