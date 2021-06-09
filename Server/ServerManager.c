#ifndef _XOPEN_SOURCE /*for signals*/
#define _XOPEN_SOURCE 500
#endif

#include <sys/socket.h> /*sockets*/
#include <stdio.h> /*prints*/
#include <string.h> /*strcmp*/
#include <errno.h> /*perror*/
#include <signal.h> /*signals*/
#include <sys/select.h> /*select*/
#include <stdlib.h>
#include "ServerNet.h"
#include "List.h"
#include "List_Itr.h"
#include "UsersTable.h"
#include "protocol.h"
#include "Groups.h"
#include "genericVector.h"

#define IP_LEN		20
#define GENERAL_LEN	40
#define BUFFER_S	4096

#define SELECT(FD_SET)			select(1024, (FD_SET), NULL, NULL, NULL);\
								do{	if((activity < 0) && (errno != EINTR)){\
								perror("select");}} while(0);

static void StartServer(void);
static void CommunicateWithClients(List* _socketList, int _listen_sock, fd_set* _master, Table* _user_table, Groups* _groups);
static void doAction(char* _buff, Table* _user_table, Groups* _groups);
static void Server_Register(char* _buff, Table* _user_table, char* username, char* password, type tag);
static void Server_LogIn(char* _buff, Table* _user_table, char* username, char* password, type tag);
static void Server_LogOut(char* _buff, Table* _user_table, char* username, char* password, type tag);
static void Server_GroupCreate(char* _buff, char* groupName, Groups* _groups, type tag);
static void Server_GroupJoin(char* _buff, char* username, char* groupName, Groups* _groups, type tag);
static void Server_GroupsNames(char* _buff, Groups* _groups);
static void Server_LeaveGroup(char* _buff, char* groupName, Groups* _groups, type tag);

int main(void)
{
	StartServer();
	return 0;
}

static void StartServer(void)
{
	fd_set master;
	int listen_sock;
	int exit = 0;
	Table* user_table = NULL;
	Groups* groups = GroupsCreate();
	
	List* socketList = List_Create();
	if(NULL == socketList)
	{
		printf("ERROR : creating list failed\n");
		return;
	}

	user_table = UserTable_CreateTable();
	if(NULL == user_table)
	{
		printf("ERROR : creating table failed\n");
		List_Destroy(&socketList, NULL);
		return;
	}

	listen_sock = ServerNet_Init(&master);

	while(!exit)
	{
		ServerNet_AcceptNewClients(socketList, listen_sock, &master);
		CommunicateWithClients(socketList, listen_sock, &master, user_table, groups);
	}

	printf("Server shutting down..\n");
	ServerNet_CloseServer(socketList, listen_sock, groups);
	List_Destroy(&socketList, NULL);
	UserTable_DestroyTable(user_table);
}

static void CommunicateWithClients(List* _socketList, int _listen_sock, fd_set* _master, Table* _user_table, Groups* _groups)
{
	ListItr iterator = NULL, begin  = ListItr_Begin(_socketList), end  = ListItr_End(_socketList);
	int activity;

	fd_set temp = *_master;
	FD_CLR(_listen_sock, &temp);

	activity = SELECT(&temp);

	for(iterator = begin; !ListItr_Equals(iterator, end) && activity > 0; iterator = ListItr_Next(iterator))
	{
		int client_sock;
		char buff[BUFFER_S];
		client_sock = *(int*)ListItr_Get(iterator);

		if(FD_ISSET(client_sock, &temp))
		{
			ServerNet_ReadFromClient(client_sock, buff, &iterator, _master);
			doAction(buff, _user_table, _groups);
			ServerNet_WriteToClient(client_sock, buff, &iterator, _master);			
		}
	}
}

void DestroyGroupName(void* _key)
{
	free(_key);
}

int	AppendFunction(const void* _key, void* _value, void* _context)
{
	Vector* vec = (Vector*)_context;
	char* groupName = (char*)malloc(strlen((char*)_key +1));
	strcpy(groupName, (char*)_key);
	Vector_Append(vec, (void*)groupName);
	return 1;
}

static void doAction(char* _buff, Table* _user_table, Groups* _groups)
{
	type tag;
	char username[GENERAL_LEN], password[GENERAL_LEN], groupName[GENERAL_LEN];

	Protocol_UnPackType(&tag, _buff);
	switch (tag)
	{
		case REQ_REG:
			Server_Register(_buff, _user_table, username, password, tag);
			break;
		
		case REQ_LOGIN:
			Server_LogIn(_buff, _user_table, username, password, tag);
			break;
			
		case REQ_LOGOUT:
			Server_LogOut(_buff, _user_table, username, password, tag);
			break;
			
		case REQ_GROUP_CREATE:
			Server_GroupCreate(_buff, groupName, _groups, tag);
			break;
		
		case REQ_GROUP_JOIN:
			Server_GroupJoin(_buff, username, groupName, _groups, tag);
			break;
			
		case REQ_GROUP_NAMES:
			Server_GroupsNames(_buff, _groups);
			break;
			
		case REQ_GROUP_LEAVE:
			Server_LeaveGroup(_buff, groupName, _groups, tag);
			break;
			
		default:
			break;
	}
}

static void Server_Register(char* _buff, Table* _user_table, char* username, char* password, type tag)
{
	Tbl_Res status;
	Protocol_UnPackUsernamePassword(&tag, username, password, _buff);
	status = UserTable_IsUserExist(_user_table, username);
	if(status == TBL_USER_EXIST)
	{
		Protocol_PackType(ERR_REG, _buff);
		return;
	}
	else
	{
		UserTable_AddUser(_user_table, username, password);
		printf("User registered : Username : %s, Password : %s\n", username, password);
		Protocol_PackType(ERR_OK, _buff);
		return;
	}
}

static void Server_LogIn(char* _buff, Table* _user_table, char* username, char* password, type tag)
{
	Tbl_Res status;
	
	Protocol_UnPackUsernamePassword(&tag, username, password, _buff);
	status = UserTable_IsMatch(_user_table, username, password);
	if (UserTable_IsActive(_user_table, username))
	{
		Protocol_PackType(ERR_ACTIVE, _buff);
	}
	else if(status == TBL_OK)
	{
		printf("User loged in : Username : %s\n", username);
		UserTable_ActivationSet(_user_table, username, 1);
		Protocol_PackType(ERR_OK, _buff);
	}
	else
	{
		Protocol_PackType(ERR_LOGIN, _buff);		
	}
}

static void Server_LogOut(char* _buff, Table* _user_table, char* username, char* password, type tag)
{
	Protocol_UnPackUsernamePassword(&tag, username, password, _buff);
	if(UserTable_IsActive(_user_table, username))
	{
		UserTable_ActivationSet(_user_table, username, 0);
		Protocol_PackType(ERR_OK, _buff);
	}
	else
	{
		Protocol_PackType(ERR_LOGOUT, _buff);
	}
}

static void Server_GroupCreate(char* _buff, char* groupName, Groups* _groups, type tag)
{
	char ip[IP_LEN];
	Protocol_UnPackGroupCreate(&tag, groupName, _buff);
	if (CreateGroup(_groups, groupName, ip) == GRP_EXIST)
	{
		Protocol_PackIP(ERR_GROUP_CREATE, "", _buff);
	}
	else
	{
		Protocol_PackIP(ERR_OK, ip, _buff);
	}
}

static void Server_GroupJoin(char* _buff, char* username, char* groupName, Groups* _groups, type tag)
{
	char ip[IP_LEN];
	Protocol_UnPackGroupJoin(&tag, groupName, username, _buff);
	if (GroupsSAddParticipant(_groups, groupName, 1, ip) == GRP_NOT_EXIST)
	{
		Protocol_PackIP(ERR_GROUP_JOIN, ip, _buff);
		return;
	}
	printf("%s Joined Group %s\n",username, groupName);
	printf("%s\n",ip);
	Protocol_PackIP(ERR_OK, ip, _buff);
}

static void Server_GroupsNames(char* _buff, Groups* _groups)
{
	Vector* vec;
	vec = Vector_Create(5,2);
	Groups_ForEach(_groups, AppendFunction, (void*)vec);
	Protocol_PackShowGroups(REQ_GROUP_NAMES, vec, _buff);
	Vector_Destroy(&vec, DestroyGroupName);
}

static void Server_LeaveGroup(char* _buff, char* groupName, Groups* _groups, type tag)
{
	char retIP[IP_LEN];
	Protocol_UnPackGroupLeave(&tag, groupName, _buff);
	Protocol_PackType(ERR_OK, _buff);
	GroupsSAddParticipant(_groups, groupName, -1, retIP);
}


