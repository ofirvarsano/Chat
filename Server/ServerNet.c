#ifndef _XOPEN_SOURCE /*for signals*/
#define _XOPEN_SOURCE 500
#endif

#include <sys/socket.h> /*sockets*/
#include <netinet/ip.h> /*sin_addrs*/
#include <stdio.h> /*printf*/
#include <string.h> /*memset*/
#include <stdlib.h> /*malloc*/
#include <unistd.h> /*close*/
#include <errno.h> /*perror*/
#include <sys/select.h> /*select*/
#include "ServerNet.h"
#include "List.h"
#include "List_Itr.h"
#include "Groups.h"

#define PORT 2231
#define BACK_LOG 1000
#define MSG_LEN_IN 200
#define MSG_LEN_OUT 200
#define ERROR 0
#define SUCCESS 1

#define BIND(SOC,SIN)	do{	(SIN).sin_family = AF_INET;\
							(SIN).sin_addr.s_addr = INADDR_ANY;\
							(SIN).sin_port = htons(PORT);\
							if(bind((SOC), (struct sockaddr*)&(SIN), sizeof(SIN)) < 0){\
							perror("bind failed");\
							return 0;}} while(0);



#define SET_SOCK_OPT(SOC,V)	do{	if(setsockopt((SOC), SOL_SOCKET, SO_REUSEADDR, &(V), sizeof((V))) < 0){\
								perror("setsockopt");\
								return 0;}} while(0);

#define LISTEN(SOC)			do{	if(listen((SOC), BACK_LOG) < 0){\
								perror("listen");\
								return 0;}} while(0);

#define SELECT(FD_SET, AC)		(AC) = select(1024, (FD_SET), NULL, NULL, NULL);\
							do{	if((activity < 0) && (errno != EINTR)){\
								perror("select");}} while(0);

typedef struct sockaddr_in sockaddr_in;
static void ServerCloseSock(int *_clientSock, fd_set* _master, ListItr *_iterator);

int ServerNet_Init(fd_set* _master)
{
	int listen_sock, optval = 1;
	sockaddr_in sin;

	memset(&sin, 0, sizeof(sin));
	listen_sock = socket(AF_INET, SOCK_STREAM,0);

	SET_SOCK_OPT(listen_sock, optval);
	BIND(listen_sock, sin);
	LISTEN(listen_sock);

	FD_ZERO(_master);
	FD_SET(listen_sock, _master);


	return listen_sock;
}

void ServerNet_CloseServer(List* _list, int _listenSock, Groups* _groups)
{
	ListItr begin = ListItr_Begin(_list), end = ListItr_End(_list), iterator = NULL;
	GroupsDestroy(_groups);

	iterator = begin;
	while (!ListItr_Equals(iterator, end))
	{
		int client_sock = *(int*)ListItr_Get(iterator);
		printf("[Socket: %d] terminated, closing..\n", client_sock);
		iterator = ListItr_Next(iterator);
		close(client_sock);
		free(ListItr_Remove(ListItr_Prev(iterator)));
	}

	close(_listenSock);
}


int ServerNet_AcceptNewClients(List* _socketList, int _listenSock, fd_set* _master)
{
	int *newClient = NULL;
	int _clientSock, sin_len, activity;
	fd_set temp = *_master;
	sockaddr_in clientSin;

	sin_len = sizeof(clientSin);	
	SELECT(&temp, activity);
	
	while(FD_ISSET(_listenSock, &temp))
	{ 	
		_clientSock = accept(_listenSock, (struct sockaddr*)&clientSin, (socklen_t*)&sin_len);
		if(_clientSock < 0)
		{
			perror("accept");
			return ERROR;
		}

		newClient = (int*)malloc(sizeof(int));
		*newClient = _clientSock;
		if(LIST_SUCCESS != List_Push_Tail(_socketList, (void*)newClient))
		{
			return ERROR;
		}	
		printf("[Socket: %d] inserted\n", _clientSock);

		FD_SET(_clientSock, _master);

		temp = *_master;
		SELECT(&temp, activity);
	}

	return SUCCESS;
}

void ServerNet_WriteToClient(int _sock, char* _buffer, ListItr *_iterator, fd_set* _master)
{
	send(_sock, _buffer, MSG_LEN_OUT, 0);
}

void ServerNet_ReadFromClient(int _sock, char* _buffer, ListItr *_iterator, fd_set* _master)
{
	if(recv(_sock, _buffer, MSG_LEN_IN, 0) < 1)
	{
		ServerCloseSock(&_sock, _master, _iterator);
	}
}


static void ServerCloseSock(int *_clientSock, fd_set* _master, ListItr *_iterator)
{
	FD_CLR(*_clientSock, _master);
	*_iterator = ListItr_Next(*_iterator);
	printf("[Socket: %d] terminated, closing..\n", *_clientSock);
	close(*_clientSock);
	free(ListItr_Remove(ListItr_Prev(*_iterator)));
	*_iterator = ListItr_Prev(*_iterator);
}

