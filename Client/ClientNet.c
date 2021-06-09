#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 500
#endif

#include <sys/socket.h> /*sockets*/
#include <netinet/ip.h> /*bind*/
#include <stdio.h> /*prints*/
#include <unistd.h> /*close*/
#include <string.h> /*memset*/
#include <arpa/inet.h> /*inet_addr()*/
#include "ClientNet.h"

int ClientNet_Init(char* _IP, int _Port)
{
	int Server_sock, sin_len;
	struct sockaddr_in sin;

	sin_len = sizeof(sin);
	memset(&sin, 0, sizeof(sin));
	Server_sock = socket(AF_INET, SOCK_STREAM,0);

	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = inet_addr(_IP);
	sin.sin_port = htons(_Port);
	if(connect(Server_sock, (struct sockaddr *)&sin, sin_len) < 0)
	{
		perror("connect");
		return ERROR;
	}

	return Server_sock;
}


int ClientNet_writeToServer(char* _buffer, int _Server_sock)
{
	if(send(_Server_sock, _buffer, MSG_LEN_OUT, 0) < 0)
	{
		return ERROR;
	}
	
	return SUCCESS;
}

int ClientNet_readFromServer(char* _buffer, int _Server_sock)
{
	if(recv(_Server_sock, _buffer, MSG_LEN_IN, 0) < 0)
	{
		return ERROR;
	}
	
	return SUCCESS;
}

void ClientNet_CloseClient(int _Server_sock)
{
	close(_Server_sock);
}
