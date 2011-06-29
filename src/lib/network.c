/*
 * Copyright (c) 2010 Ignasi Barrera
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include "network.h"

struct hostent *host_entry; // Host name
struct sockaddr_in sock_addr; // Remote address


void net_connect(char* address, int port) {
	// Socket creation
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket creation error");
		exit(EXIT_FAILURE);
	}

	// Write zeros into remote address structure
	memset(&sock_addr, 0, sizeof(sock_addr));

	// Family type and server port
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(port);

	// Get remote host address
	if ((host_entry = gethostbyname(address)) == NULL) {
		perror("gethostbyname error");
		exit(EXIT_FAILURE);
	}

	// Save remote host address
	sock_addr.sin_addr = *((struct in_addr *) host_entry->h_addr);

	// Create a connection with the remote host
	if (connect(s, (struct sockaddr *) &sock_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect error");
		exit(EXIT_FAILURE);
	}
}

void net_disconnect() {
	close(s);
	s = -1;
}

int net_send(char* msg) {
	char out[MSG_SIZE];

	strncpy(out, msg, MSG_SIZE - 2); // Cut the message to the maximum size
	strcat(out, "\r\n"); // Messages must end like this

	printf(">> %s\n", msg);

	return send(s, out, strlen(out), 0);
}

int net_recv(char* msg) {
	int numbytes;

	if ((numbytes = recv(s, msg, MSG_SIZE - 1, 0)) > 0) {
		msg[numbytes] = '\0'; // Append the "end of string" character
	}

	printf("<< %s\n", msg);

	return numbytes;
}
