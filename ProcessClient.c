// Simple Windows Sockets Echo Client and Server
// Dave Heldenbrand 6/29/12
//
// This processes the clients message and sends it back to the client
// ----------------------------------------------------------------------------
// Minimum required header files for C Winsock program
#include <stdio.h> // for print functions
#include <stdlib.h> // for exit()
#include <winsock2.h> // for WSAGetLastError()
void ProcessClient(int clientSock, char* rcvBuffer, int bufferSize)
{
	int recvMsgSize;
	while (1){
		recvMsgSize = recv(clientSock, rcvBuffer, bufferSize - 1, 0);
		if (recvMsgSize == 0) {
			printf("Full message sent to client successfully\n");
			printf("Socket closed by server!\n");
			break;
		}
		//message error!
		if (recvMsgSize < 0) {
			printf("Message Error!\n");
			break;
		}
		send(clientSock, rcvBuffer, recvMsgSize, 0);
	}

	closesocket(clientSock);//close socket and send FIN
	printf("Client Socket Closed Successfully...\n");
	printf("Ready for next client!\n");
}