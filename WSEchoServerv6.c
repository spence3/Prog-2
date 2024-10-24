// CS 2690 Program 1
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 2/12/19
// Spencer Miller 601 10/11/24
// Windows 11 VS 2019
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero
// on this project if I violate this policy.
// ----------------------------------------------------------------------------

#include <stdio.h> // for print functions
#include <stdlib.h> // for exit()
#include <winsock2.h> // for Winsock2 functions
#include <ws2tcpip.h> // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h> // optional - needed for MS IP Helper
#include <stdbool.h> //include bool

// #define ALL required constants HERE, not inline
// #define is a macro, don't terminate with ';' For example...
#define RCVBUFSIZ 50 // buffer size for received messages
const char rcvBuffer[RCVBUFSIZ]; // Buffer to store received data

// declare any functions located in other .c files here
#ifndef DISPLAYFATALERR_H
#define DISPLAYFATALERR_H
void DisplayFatalErr(const char* errMsg);
#endif // !DISPLAYFATALERR_H

#ifndef PROCESSCLIENT_H
#define PROCESSCLIENT_H
void ProcessClient(int clientSock, char* rcvBuffer, int bufferSize);
#endif // !DISPLAYFATALERR_H


int main(int argc, char* argv[]) {
    // Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
    WSADATA wsaData; // contains details about WinSock DLL implementation
    //initialize data
    int numArgs = argc;
    //int MAXQUEUED = 128; // maximum simultaneous client connection requests.
    int port;
    int serverSock;
    struct sockaddr_in6 clientInfo; //Holds client port & addr after client connects
    int clientInfoLen;
    int clientSock;
    //verify the correct number of command line arguments have been provided by the user.
    if (numArgs != 2) {
        printf("Number of arguments are not corret... Must have 2.");
        exit(1);
    }
    //default port
    port = 2000;
    //if port is input correctly
    if ((unsigned short)atoi(argv[1])) {
        port = (unsigned short)atoi(argv[1]);
    }
    
    //initialize Winsock
    if ((WSAStartup(MAKEWORD(2, 0), &wsaData)) != 0) {
        DisplayFatalErr("Winsock Unsuccessfully Loaded");
    }
    
    serverSock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP); // Create server socket

    //server info
    struct sockaddr_in6 serverInfo;
    memset(&serverInfo, 0, sizeof(serverInfo)); // zero out the structure

    //load server info into sockadd_in6
    serverInfo.sin6_family = AF_INET6;
    serverInfo.sin6_port = htons(port); //convert int port into network order*
    serverInfo.sin6_addr = in6addr_any; //Use in6addr_any as the server’s IP address

    //bind() the server socket to this sockaddr_in6 structure.
    if (bind(serverSock, (struct sockaddr*)&serverInfo, sizeof(serverInfo)) != 0) {
        DisplayFatalErr("Bind failed");
    }

    //maximum simultaneous client connection requests allowed.
    listen(serverSock, SOMAXCONN);

    printf("SRM's IPv6 echo server is ready for client connection...\n");

    for (;;) {
        clientInfoLen = sizeof(clientInfo);
        clientSock = accept(serverSock, (struct sockaddr*)&clientInfo, &clientInfoLen);
        
        //display the IP address and port number of the client, and the server’s own port number
        char clientIP[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, &clientInfo.sin6_addr, clientIP, sizeof(clientIP));
        unsigned short clientPort = ntohs(clientInfo.sin6_port);
        printf("Proeccessing the client at %s, client port %d, server port %d\n", clientIP, clientPort, port);

        //Echos message back to client
        ProcessClient(clientSock, rcvBuffer, RCVBUFSIZ);
    }
    exit(0);
}


//DONE:
/*
1. The primary source code file will be named WSEchoServerv6.c. This will contain your
main() function. As with your client, verify the correct number of command line
arguments have been provided by the user. You don’t need to validate the content of
those arguments. If no port number is included on the command line, you may use a
default port of your own choosing.

2. Initialize the WinSock DLL. After a successful call to WSAStartup(), handle any errors by
calling DisplayFatalErr().

3. Create the server socket.

4. Load the server information into the server’s sockaddr_in6 structure (see Lecture 12
slide titled “Server Socket Initialization”). Use in6addr_any as the server’s IP
address.

5. bind() the server socket to this sockaddr_in6 structure.

6. Call listen() to tell the server the maximum simultaneous client connection requests
to allow.

7. Display a message on the server console that includes your initials, similar to this:
JD's IPv6 echo server is ready for client connection...

8. Enter a “forever” loop like this:
for (;;) { ... }
From within this loop, call accept() and wait for a client connection. Calling
accept() from within this for loop will allow the server to accept connection requests
from one or more clients, but only one at a time. Echo back one complete client
message per connection request. (This non-threaded program will not support multiple
simultaneous client connections.)

9. Each time a client connects to the server in this loop, display the IP address and port
number of the client, and the server’s own port number.

10. From within the “forever” loop, call a function named ProcessClient() to receive the
message from your client. Implement ProcessClient() in a separate source code file
named ProcessClient.c. Declare ProcessClient() in WSEchoServerv6.c, outside of
main(), the same way that DisplayFatalErr() is declared. Check for errors in the
Sockets receive process, as indicated by a negative return value from recv().

11. Echo the message back to the client from within ProcessClient(). Be sure that all of
the client’s message has been received and echoed by the server. You can’t count bytes
as you did with the client, because the server doesn’t know in advance how many bytes
to expect from the client, and whether all of those bytes will be received in the initial call
to recv(). So call recv() repeatedly until recv() returns message length 0,
indicating that the client has closed the connection. (See the Lecture 11 slide titled
“recv( )” for details.)

12. While still in ProcessClient(), close the client socket, return to the “forever” loop in
WSEchoServerv6.c, then call accept() and wait for another client connection.

13. While waiting in the “forever” loop for client communication, the server can be terminated
with CTRL+C. No special programming is required to enable this. Note that CTRL+C
works when running the server from the command line, but not if you are running it in the
Visual Studio debugger. Because we are using this crude termination mechanism for the
server, no call to closesocket() will be needed to close the server socket and no call
to WSACleanup() will be needed to release the WinSock DLL resources. Those
functions will be handled by the operating system. (In a real sockets program, omitting
closesocket() and WSACleanup() would be considered poor design.)
*/
