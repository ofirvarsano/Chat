#include <string.h>
#include <stdio.h> 
#include <stdlib.h>
#include "protocol.h"

void Protocol_PackUsernamePassword(type _tag, const char* _userName, const char* _password, char* _buffer)
{
	int userNameLen = strlen(_userName), passwordLen = strlen(_password);
	_buffer[0] = (int)_tag;	
	_buffer[1] = (int)userNameLen;
	memcpy(_buffer + 2, _userName, userNameLen * sizeof(char));
	_buffer[2 + userNameLen] = (int)passwordLen;
	memcpy(_buffer + 3 + userNameLen, _password, passwordLen * sizeof(char));
}

void Protocol_UnPackUsernamePassword(type* _tag, char* _username, char* _password, const char* _buffer)
{
	int usernameLen = (int)_buffer[1], passwordLen = (int)_buffer[2 + usernameLen];
	*_tag = _buffer[0];
	memcpy(_username, _buffer + 2, usernameLen * sizeof(char));
	_username[usernameLen] = 0;
	memcpy(_password, _buffer + 3 + usernameLen, passwordLen * sizeof(char));
	_password[passwordLen] = 0;
}

void Protocol_PackType(type _tag, char* _buffer)
{
	_buffer[0] = (int)_tag;	
}

void Protocol_UnPackType(type* _tag, const char* _buffer)
{
	*_tag = _buffer[0];
}

void Protocol_PackGroupCreate(type _tag, char* _buffer, const char* _groupName)
{
	int groupNameLen = strlen(_groupName);
	_buffer[0] = (int)_tag;
	_buffer[1] = (int)groupNameLen;
	memcpy(_buffer + 2, _groupName, groupNameLen * sizeof(char));
}

void Protocol_UnPackGroupCreate(type* _tag, char* _groupName, const char* _buffer)
{
	int groupNameLen = (int)_buffer[1];
	*_tag = _buffer[0];
	memcpy(_groupName, _buffer + 2, groupNameLen * sizeof(char));
	_groupName[groupNameLen] = 0;
}

void Protocol_PackGroupJoin(type _tag, char* _buffer, const char* _groupName, const char* _userName)
{
	int groupNameLen = strlen(_groupName);
	int userNameLen = strlen(_userName);
	_buffer[0] = (int)_tag;
	_buffer[1] = (int)groupNameLen;
	memcpy(_buffer + 2, _groupName, groupNameLen * sizeof(char));
	_buffer[2 + groupNameLen] = (int)userNameLen;
	memcpy(_buffer + 3 + groupNameLen, _userName, userNameLen * sizeof(char));
}

void Protocol_UnPackGroupJoin(type* _tag, char* _groupName, char* _userName, const char* _buffer)
{
	int groupNameLen = (int)_buffer[1], userNameLen = (int)_buffer[2 + groupNameLen];
	*_tag = _buffer[0];
	memcpy(_groupName, _buffer + 2, groupNameLen * sizeof(char));
	_groupName[groupNameLen] = 0;
	memcpy(_userName, _buffer + 3 + groupNameLen, userNameLen * sizeof(char));
	_userName[userNameLen] = 0;
}

void Protocol_PackIP(type _tag, const char* _IP, char* _buffer)
{
	int _IPlen = strlen(_IP);
	_buffer[0] = _tag;	
	_buffer[1] = _IPlen;	
	memcpy(_buffer + 2, _IP, _IPlen * sizeof(char));
}

void Protocol_UnPackIP(type* _tag, char* _IP, const char* _buffer)
{
	int _IPlen = (int)_buffer[1];
	*_tag = _buffer[0];
	memcpy(_IP, _buffer + 2, _IPlen * sizeof(char));
	_IP[_IPlen] = 0;
}

void Protocol_PackGroupLeave(type _tag, char* _buffer, const char* _groupName)
{
	int groupNameLen = strlen(_groupName);
	_buffer[0] = (int)_tag;	
	_buffer[1] = (int)groupNameLen;
	memcpy(_buffer + 2, _groupName, groupNameLen * sizeof(char));
}

void Protocol_UnPackGroupLeave(type* _tag, char* _groupName, const char* _buffer)
{
	int groupNameLen = (int)_buffer[1];
	*_tag = _buffer[0];
	memcpy(_groupName, _buffer + 2, groupNameLen * sizeof(char));
	_groupName[groupNameLen] = 0;
}

void Protocol_PackShowGroups(type _tag, Vector *_vec, char *_buffer)
{
    char *numOfGroupsI;
    char *groupName;
	int size = 2;
    int groupLen = 0, numOfGroups = 0;
    _buffer[0] = (int)_tag;
	numOfGroupsI = _buffer + 1;

    while (Vector_Remove(_vec, (void **)&groupName) == VECTOR_SUCCESS)
    {
        groupLen = strlen(groupName);

        _buffer[size] = (char)groupLen;
        memcpy(_buffer + size + 1, groupName, groupLen * sizeof(char));
        ++numOfGroups;
        free(groupName);
		size += groupLen + 1;
    }
    *numOfGroupsI = (char)numOfGroups;
}

void Protocol_UnPackShowGroups(type *_tag, Vector *_vec, const char *_buffer)
{
    char *groupName;
	int size;
    int numOfGroups = (int)_buffer[1];
    int groupLen;
    *_tag = _buffer[0];
	size = 2;

    while (numOfGroups > 0)
    {
        groupLen = (int)_buffer[size];
        groupName = (char *)calloc((int)groupLen + 1, sizeof(char));
        memcpy(groupName, _buffer + size + 1, groupLen * sizeof(char));
        groupName[groupLen] = '\0';
        Vector_Append(_vec, groupName);
        --numOfGroups;
		size += 1 + groupLen;
    }
}
