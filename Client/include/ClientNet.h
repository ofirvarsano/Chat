#ifndef CLIENT_NET_H
#define CLIENT_NET_H

#define BACK_LOG 1000
#define MSG_LEN_IN 200
#define MSG_LEN_OUT 200
#define ERROR 0
#define SUCCESS 1

/*
Description: Function create server socket and makes a connect.
Input: Char pointer of the ip adress and the port.
Output: The socket created.
Complexity: o(1).
*/
int ClientNet_Init(char* _IP, int _Port);

/*
Description: Function write to server.
Input: Char pointer to the buffer and the server socket.
Output: SUCCESS on success and ERROR on fail.
Complexity: o(1).
*/
int ClientNet_writeToServer(char* _buffer, int _Server_sock);

/*
Description: Function read from server.
Input: Char pointer to the buffer and the server socket.
Output: SUCCESS on success and ERROR on fail.
Complexity: o(1).
*/
int ClientNet_readFromServer(char* _buffer, int _Server_sock);

/*
Description: Function close the selected socket.
Input: The socket selected.
Output: void.
Complexity: o(1).
*/
void ClientNet_CloseClient(int _Server_sock);

#endif /*CLIENT_NET_H*/
