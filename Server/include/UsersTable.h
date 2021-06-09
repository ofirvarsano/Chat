#ifndef USER_TABLE_H
#define USER_TABLE_H
#include "Groups.h"

typedef struct Table Table;
typedef struct User User;

typedef enum
{
	TBL_OK,
	TBL_USER_EXIST,
	TBL_USER_NOT_EXIST,
	TBL_USER_NO_MATCH,
	TBL_ERR
}Tbl_Res;

/*
Description: Function create new user table.
Input: void.
Output: Pointer to new Table on success and NULL on error.
Complexity: o(1).
*/
Table*	UserTable_CreateTable();

/*
Description: Function add a new user to table.
Input: _tbl - pointer to table, _username & _password - arguments to add into table.
Output: TBL_USER_EXIST - if username allready exist, TBL_ERR - on error and TBL_OK on success.
Complexity: o(1).
*/
Tbl_Res	UserTable_AddUser(Table* _tbl, char* _username, char* _password);

/*
Description: Function check if given username matches given password in the table.
Input: _tbl - pointer to table, _username & _password - arguments to check.
Output: TBL_USER_NOT_EXIST - if username not exist, TBL_USER_NO_MATCH - if password doest match the username and TBL_OK on success.
Complexity: o(1).
*/
Tbl_Res UserTable_IsMatch(Table* _tbl, char* _username, char* _password);

/*
Description: Function check if given username exist in the table.
Input: _tbl - pointer to table, _username argument to check.
Output: TBL_USER_NOT_EXIST - if username not exist and TBL_USER_EXIST if exist.
Complexity: o(1).
*/
Tbl_Res UserTable_IsUserExist(Table* _tbl, char* _username);

/*
Description: Function destroys given table.
Input: _tbl - pointer to table.
Output: Void.
Complexity: o(n).
*/
void UserTable_DestroyTable(Table* _tbl);

/*
Description: Function check if user is active.
Input: _tbl - pointer to table and username. 
Output: 1 if active and 0 if not active.
Complexity: o(1).
*/
int UserTable_IsActive(Table* _tbl, char* _username);

/*
Description: Function sets activation mode.
Input: _tbl - pointer to table, _username- char pointer to username and integer for activation. 
Output: Void.
Complexity: o(1).
*/
void UserTable_ActivationSet(Table* _tbl, char* _username, int _isActive);

#endif /*USER_TABLE_H*/
