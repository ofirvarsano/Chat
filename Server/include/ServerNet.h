#ifndef SERVER_NET_H
#define SERVER_NET_H

#include <sys/select.h>
#include "List.h"
#include "List_Itr.h"
#include "Groups.h"


/*
Description: Function initialize the server.
Input: fd_set pointer.
Output: Listening socket of server.
Complexity: o(1).
*/
int ServerNet_Init(fd_set* _master);

/*
Description: Function accepet new clients to server.
Input: fd_set pointer to edit with new clients sockets, _socketList to add the new clients sockets, _listenSock - server's listening socket.
Output: 0 on fail and 1 on success.
Complexity: o(1).
*/
int ServerNet_AcceptNewClients(List* _socketList, int _listenSock, fd_set* _master);

/*
Description: Function write buffer to a given client socket.
Input: _sock - client socket. _buffer - buffer to send, _iterator - iterator of current client to destroy if client closed the socket. _master - to clear client socket if client.
Output: Void.
Complexity: o(1).
*/
void ServerNet_WriteToClient(int _sock, char* _buffer, ListItr *_iterator, fd_set* _master);

/*
Description: Function write buffer to a given client socket.
Input: _sock - client socket. _buffer - buffer to read to, _iterator - iterator of current client to destroy if client closed the socket. _master - to clear client socket if client closed the socket.
Output: Void.
Complexity: o(1).
*/
void ServerNet_ReadFromClient(int _sock, char* _buffer, ListItr *_iterator, fd_set* _master);
/*
Description: Function closes and free all clients from list.
Input: _list and _listenSock to close.
Output: Void.
Complexity: o(1).
*/
void ServerNet_CloseServer(List* _list, int _listenSock, Groups* _groups);

#endif /*SERVER_NET_H*/
