/*
 * Copyright (c) 2011 Ignasi Barrera
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

#ifndef __NETWORK_H__
#define __NETWORK_H__

#define MSG_SIZE 512    // The maximum message size of an IRC message
#define MSG_SEP "\r\n"  // The message separator

#define READ_BUF (MSG_SIZE + 1)     // The read buffer size
#define WRITE_BUF (MSG_SIZE - 2)    // The write buffer size

// Network status
enum net_status {
    NET_READY,      // There is data to be read from the socket
    NET_ERROR,      // Unexpected error while reading
    NET_CLOSE,      // The connection must terminate
    NET_IGNORE      // Ignore the status and continue
};

// Network functions. Allow them to be redefined or mocked for testing purposes
void net_connect(char* address, int port);  // Connect to the IRC server
void net_disconnect();                      // Disconnect from the server
void net_recv(char* msg);                   // Receive a message from the server
enum net_status net_listen();               // Listen for incoming messages

#ifdef __GNUC__
// Not portable. Try to define the weak attribute to allow to redefine the
// functions and mock them in tests.
__attribute__((weak)) int net_send(char* msg);   // Send a message to the server
#else
int net_send(char* msg);   // Send a message to the server
#endif

#endif

