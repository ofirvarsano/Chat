#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include "ClientNet.h"
#include "protocol.h"
#include "Client_GUI.h"
#include "genericVector.h"
#include <unistd.h>
#include "List.h"
#include "List_Itr.h"

#define SLEEP_TIME		2
#define BUFFER_LEN		200
#define LEN				50
#define IP_ADDRESS		"10.0.2.15"
#define PORT			2231

typedef struct UserGroup UserGroup;

static void StartClient(void);
static int ClientManager_Login(int _ServerSock, char _username[LEN], char _password[LEN], int* userLog, List** userGroups);
static int ClientManager_Register(int _ServerSock, char _username[LEN], char _password[LEN], int *userLog, List** userGroups);
static void ClientManager_LogOut(int _ServerSock, char _username[LEN], int *userLog, List** userGroups);
static void ClientManager_HomeSwitch(int _ServerSock, char _username[LEN], Choice choice, int *userLog, List** userGroups);
static void ClientManager_GroupSwitch(int _ServerSock, char _username[LEN], Choice choice, int *userLog, List** userGroups);
void destroyUserGroup(void* group);
int isClientInGroup(List* userGroups, char* groupName);

struct UserGroup
{
	char* m_groupName;
	char* m_ipAdd;
	int m_pidClient;
	int m_pidServer;
};

int main(void)
{
	StartClient();
	return 0;
}

static void StartClient(void)
{
	int Server_sock;
	Choice choice;
	char username[LEN];
	int userLog = 0;
	List* userGroups = NULL;
	/*
	Server_sock = ClientNet_Init("192.168.1.23", 2231);
	*/
	Server_sock = ClientNet_Init(IP_ADDRESS, PORT);
	
	do
	{
		choice = ClientGUI_Homepage();
		ClientManager_HomeSwitch(Server_sock, username, choice, &userLog, &userGroups);
		if (choice == EXIT)
		{
			break;
		}
		
	}while (choice != EXIT);
	
	ClientManager_LogOut(Server_sock, username, &userLog, &userGroups);
	ClientNet_CloseClient(Server_sock);
	List_Destroy(&userGroups, destroyUserGroup);

}

static void join(char* buffer, char* groupName, char* _username, int _ServerSock, List** userGroups)
{
	type status;
	FILE* fptr, *fptr2;
	char ip[GROUP_NAME_LEN], command[BUFFER_LEN];
	int clientPID, serverPID;
	UserGroup* newUserGroup;
	
	if (isClientInGroup(*userGroups, groupName))
	{
		printf("ERR GROUP JOIN : you are already in group %s\n", groupName);
		sleep(SLEEP_TIME);
		return;
	}
	Protocol_PackGroupJoin(REQ_GROUP_JOIN, buffer, groupName, _username);

	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);

	Protocol_UnPackIP(&status, ip, buffer);
	if(status == ERR_GROUP_JOIN)
	{
		printf("ERR GROUP JOIN : group not found, please try again\n");
	}
	else
	{
		printf("User joined group : %s!\n", groupName);

		sprintf(command, "gnome-terminal -q -- ./ChatWindow/Listener.out %s %s",ip, groupName);
		system(command);

		fptr = fopen("pidS.txt","r");
		fscanf(fptr,"%d",&serverPID);
		fclose(fptr);

		sprintf(command, "gnome-terminal -q -- ./ChatWindow/Sender.out %s %s %s",ip, _username, groupName);
		system(command);

		fptr2 = fopen("pidC.txt","r");
		fscanf(fptr2,"%d",&clientPID);
		fclose(fptr2);

		newUserGroup = (UserGroup*)malloc(sizeof(UserGroup));
		newUserGroup->m_groupName = (char*)malloc(strlen(groupName)+1);
		strcpy(newUserGroup->m_groupName, groupName);
		newUserGroup->m_ipAdd = (char*)malloc(strlen(ip)+1);
		strcpy(newUserGroup->m_ipAdd, ip);

		newUserGroup->m_pidClient = clientPID;
		newUserGroup->m_pidServer = serverPID;
		
		List_Push_Tail(*userGroups, newUserGroup);
	}
	sleep(SLEEP_TIME);
}

static void ClientManager_CreateGroup(int _ServerSock, char _username[LEN], List** userGroups)
{
	type status;
	char groupName[GROUP_NAME_LEN], buffer[BUFFER_LEN], ip[GROUP_NAME_LEN];

	printf("Insert Group Name to create\n");
	scanf("%s",groupName);

	Protocol_PackGroupCreate(REQ_GROUP_CREATE, buffer, groupName);

	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);

	Protocol_UnPackIP(&status, ip, buffer);

	if(status == ERR_GROUP_CREATE)
	{
		printf("ERR GROUP CREATE : Group Name is taken!\n");
		sleep(SLEEP_TIME);
	}
	else
	{
		printf("Group %s Created!\n", groupName);
		join(buffer, groupName, _username, _ServerSock, userGroups);
	}
}

static void ClientManager_JoinGroup(int _ServerSock, char _username[LEN], List** userGroups)
{
	char groupName[GROUP_NAME_LEN], buffer[BUFFER_LEN];
	printf("Insert Group Name to join to\n");
	scanf("%s",groupName);
	join(buffer, groupName, _username, _ServerSock, userGroups);
}

void DestroyGroupName(void* _key)
{
	free(_key);
}

static void ClientManager_ShowGroups(int _ServerSock)
{
	char buffer[BUFFER_LEN];
	type status;
	char* groupPtr;
	int i = 1;
	Vector* vec = Vector_Create(5,2);
	Protocol_PackType(REQ_GROUP_NAMES, buffer);
	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);

	Protocol_UnPackShowGroups(&status, vec, buffer);

	while(Vector_Remove(vec, (void**)&groupPtr) == VECTOR_SUCCESS)
	{
		printf("%d. %s\n", i, groupPtr);
		++i;
		free(groupPtr);
	}
	Vector_Destroy(&vec, DestroyGroupName);
}

static void ClientManager_RemoveGroup(char _groupName[LEN], List** userGroups)
{
	ListItr iterator, begin, end;
	UserGroup* currentGroup;

	begin  = ListItr_Begin(*userGroups);
	end  = ListItr_End(*userGroups);

	iterator = begin;
	while (!ListItr_Equals(iterator, end))
	{
		currentGroup = (UserGroup*)ListItr_Get(iterator);
		if (strcmp(currentGroup->m_groupName, _groupName) == 0)
		{
			iterator = ListItr_Next(iterator);
			kill(currentGroup->m_pidClient, SIGKILL);
			kill(currentGroup->m_pidServer, SIGKILL);
			destroyUserGroup(ListItr_Remove(ListItr_Prev(iterator)));
			return;
		}
		iterator = ListItr_Next(iterator);
	}
}

static void ClientManager_RemoveAllGroups(int _ServerSock, List** userGroups)
{
	ListItr iterator, begin, end;
	UserGroup* currentGroup;
	char buffer[BUFFER_LEN];
	if (*userGroups == NULL)
	{
		return;
	}
	begin  = ListItr_Begin(*userGroups);
	end  = ListItr_End(*userGroups);

	iterator = begin;
	while (!ListItr_Equals(iterator, end))
	{
		currentGroup = (UserGroup*)ListItr_Get(iterator);

		Protocol_PackGroupLeave(REQ_GROUP_LEAVE, buffer, currentGroup->m_groupName);
		ClientNet_writeToServer(buffer, _ServerSock);
		ClientNet_readFromServer(buffer, _ServerSock);

		iterator = ListItr_Next(iterator);
		kill(currentGroup->m_pidClient, SIGKILL);
		kill(currentGroup->m_pidServer, SIGKILL);
		destroyUserGroup(ListItr_Remove(ListItr_Prev(iterator)));
	}
}

static void ClientManager_LeaveGroup(int _ServerSock, List** userGroups)
{
	type status;
	char groupName[GROUP_NAME_LEN], buffer[BUFFER_LEN];
	
	printf("Insert Group Name to leave\n");
	scanf("%s",groupName);
	
	if (!isClientInGroup(*userGroups, groupName))
	{
		printf("ERR LEAVE GROUP : you are not in group %s\n", groupName);
		sleep(SLEEP_TIME);
		return;
	}
	Protocol_PackGroupLeave(REQ_GROUP_LEAVE, buffer, groupName);
	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);
	Protocol_UnPackType(&status, buffer);
	
	if(status == ERR_GROUP_LEAVE)
	{
		printf("ERR GROUP LEAVE : There is no such group!\n");
	}
	else
	{
		ClientManager_RemoveGroup(groupName, userGroups);
		printf("You have left the group\n");
	}
	sleep(SLEEP_TIME);
}

static void ClientManager_HomeSwitch(int _ServerSock, char _username[LEN], Choice choice, int *userLog, List** userGroups)
{
	char password[LEN];
	switch (choice)
	{
		case SIGN_IN:
			ClientGUI_GetUserDetails(_username, password);
			if (ClientManager_Login(_ServerSock, _username, password, userLog, userGroups))
			{
				do
				{
					choice = ClientGUI_GroupMenu();
					ClientManager_GroupSwitch(_ServerSock, _username, choice, userLog, userGroups);
				}while (choice != EXIT && choice != LOG_OUT);
			}
			break;
		case REGISTER:
			ClientGUI_GetUserDetails(_username, password);
			if (ClientManager_Register(_ServerSock, _username, password, userLog, userGroups))
			{
				do
				{
					choice = ClientGUI_GroupMenu();
					ClientManager_GroupSwitch(_ServerSock, _username, choice, userLog, userGroups);
				}while (choice != EXIT && choice != LOG_OUT);
			}
			break;
			
		case LOG_OUT:
			ClientManager_LogOut(_ServerSock, _username, userLog, userGroups);
			break;
			
		default:
			break;	
	}
}

static void ClientManager_GroupSwitch(int _ServerSock, char _username[LEN], Choice choice, int *userLog, List** userGroups)
{
	switch (choice)
	{
	
		case CREATE_GROUP:
			ClientManager_CreateGroup(_ServerSock, _username, userGroups);
			break;
		
		case JOIN_GROUP:
			ClientManager_ShowGroups(_ServerSock);
			ClientManager_JoinGroup(_ServerSock, _username, userGroups);
			break;			 
	
		case LEAVE_GROUP:
			ClientManager_LeaveGroup(_ServerSock, userGroups);
			break;
		
		case LOG_OUT:
			ClientManager_LogOut(_ServerSock, _username, userLog, userGroups);
			break;

		default :
			break;
	}
}

void destroyUserGroup(void* group)
{
	if (group)
	{
		free(((UserGroup*)group)->m_groupName);
		free(((UserGroup*)group)->m_ipAdd);
		free(group);
	}
}

static void ClientManager_LogOut(int _ServerSock, char _username[LEN], int *userLog, List** userGroups)
{
	type status;
	char password[LEN], buffer[BUFFER_LEN];

	Protocol_PackUsernamePassword(REQ_LOGOUT, _username, password, buffer);
	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);
	Protocol_UnPackType(&status, buffer);
	if(status == ERR_OK)
	{
		printf("User %s Loged out!\n", _username);
		ClientManager_RemoveAllGroups(_ServerSock, userGroups);
		List_Destroy(userGroups, destroyUserGroup);
		*userLog = 0;
		sleep(SLEEP_TIME);
	}
}


static int ClientManager_Register(int _ServerSock, char _username[LEN], char _password[LEN], int *userLog, List** userGroups)
{
	type status;
	char buffer[BUFFER_LEN];

	do
	{
		Protocol_PackUsernamePassword(REQ_REG, _username, _password, buffer);
		ClientNet_writeToServer(buffer, _ServerSock);
		ClientNet_readFromServer(buffer, _ServerSock);
		Protocol_UnPackType(&status, buffer);
		if(status == ERR_REG)
		{
			printf("ERR REGISTER : username is occuppied, please try again\n");
			ClientGUI_GetUserDetails(_username, _password);
		}
	}while(status == ERR_REG);
	printf("User %s Registerd!\n", _username);
	
	return ClientManager_Login(_ServerSock, _username, _password, userLog, userGroups);
}

static int ClientManager_Login(int _ServerSock, char _username[LEN], char _password[LEN], int* userLog, List** userGroups)
{
	type status;
	char buffer[BUFFER_LEN];

	Protocol_PackUsernamePassword(REQ_LOGIN, _username, _password, buffer);
	ClientNet_writeToServer(buffer, _ServerSock);
	ClientNet_readFromServer(buffer, _ServerSock);
	Protocol_UnPackType(&status, buffer);
	if(status == ERR_LOGIN)
	{
		printf("ERR LOGIN : user not found, please try again\n");
		sleep(SLEEP_TIME);
		return 0;
	}
	else if(status == ERR_ACTIVE)
	{
		printf("ERR ACTIVE : user is already active!\n");
		sleep(SLEEP_TIME);
		return 0;
	}
	printf("User %s Loged in!\n", _username);
	*userLog = 1;
	*userGroups = List_Create();
	return 1;
}

int isClientInGroup(List* userGroups, char* groupName)
{
	ListItr iterator, begin, end;
	UserGroup* currentGroup;
	if (userGroups == NULL)
	{
		return 0;
	}

	begin  = ListItr_Begin(userGroups);
	end  = ListItr_End(userGroups);

	iterator = begin;
	while (!ListItr_Equals(iterator, end))
	{
		currentGroup = (UserGroup*)ListItr_Get(iterator);
		if (strcmp(currentGroup->m_groupName, groupName) == 0)
		{
			return 1;
		}
		iterator = ListItr_Next(iterator);
	}
	return 0;
}



