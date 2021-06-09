#ifndef CHAT_PROTOCOL_H
#define CHAT_PROTOCOL_H

#define GROUP_NAME_LEN 20
#define NUM_OF_GROUPS  50

#include "HashMap.h"
#include "genericVector.h"

typedef enum{
	ERR_OK
	,REQ_REG
	,ERR_REG
	
	,REQ_LOGIN
	,ERR_LOGIN
	,ERR_ACTIVE
	
    ,REQ_LOGOUT
    ,ERR_LOGOUT
    
	,REQ_GROUP_CREATE
	,ERR_GROUP_CREATE
	
	,REQ_GROUP_JOIN
	,ERR_GROUP_JOIN
	
	,ADD_GROUP_USER
	
	,REQ_GROUP_NAMES
	
    ,REQ_GROUP_LEAVE
	,ERR_GROUP_LEAVE

} type;

/*
Description: Function pack the username and password.
Input: Action type, char pointer to username, password and buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackUsernamePassword(type _tag, const char* _username, const char* _password, char* _buffer);

/*
Description: Function unpack the action, username and password from buffer.
Input: Action type, char pointer to username, password and buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackUsernamePassword(type* _tag, char* _username, char* _password, const char* _buffer);

/*
Description: Function pack action type.
Input: Action type and constant char buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackType(type _tag, char* _buffer);

/*
Description: Function unpack action type from buffer.
Input: Action type and constant char buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackType(type* _tag, const char* _buffer);

/*
Description: Function pack the ip and action type.
Input: Action type, constant char pointer to the ip addres and char pointer to buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackIP(type _tag, const char* _IP, char* _buffer);

/*
Description: Function unpack the buffer from buffer.
Input: Action type, constant char pointer to the ip addres and char pointer to buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackIP(type* _tag, char* _IP, const char* _buffer);

/*
Description: Function pack the data of user that want to create new group.
Input: Action type, constant char pointer to the group name and char pointer to buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackGroupCreate(type _tag, char* _buffer, const char* _groupName);

/*
Description: Function unpack the group name and action from buffer
Input: Action type, constant char pointer to the group name and char pointer to buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackGroupCreate(type* _tag, char* _groupName, const char* _buffer);

/*
Description: Function unpack the data of a user that want to join to new group.
Input: Action type, constant char pointer to the buffer and  char pointer to username and group name.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackGroupJoin(type* _tag, char* _groupName, char* _userName, const char* _buffer);

/*
Description: Function pack the data of a user that want to join to new group.
Input: Action type, constant char pointer group name and username and char pointer to the buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackGroupJoin(type _tag, char* _buffer, const char* _groupName, const char* _userName);

/*
Description: Function pack the data of a user that want to leave a group.
Input: Action type, char pointer to the buffer and constant char pointer group name.
Output: void.
Complexity: o(1).
*/
void Protocol_PackGroupLeave(type _tag, char* _buffer, const char* _groupName);

/*
Description: Function unpack the data of a user that want to leave a group.
Input: Action type, char pointer to group name and constant char pointer to the buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackGroupLeave(type* _tag, char* _groupName, const char* _buffer);

/*
Description: Function pack groups names fro, vector.
Input: Action type, poinet to vector and char pointer to the buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_PackShowGroups(type _tag, Vector *_vec, char *_buffer);

/*
Description: Function unpack groups names from buffer to given vector.
Input: Action type, poinet to vector and constant char pointer to the buffer.
Output: void.
Complexity: o(1).
*/
void Protocol_UnPackShowGroups(type *_tag, Vector *_vec, const char *_buffer);

#endif /*CHAT_PROTOCOL_H*/
