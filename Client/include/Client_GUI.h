#ifndef CLIENT_GUI_H
#define CLIENT_GUI_H

#include "protocol.h"

#define BUFFER_SIZE 20

typedef enum{
	 EXIT 
	,REGISTER
	,SIGN_IN
	,LOG_OUT
	,CREATE_GROUP
	,JOIN_GROUP
	,LEAVE_GROUP
	,INVALID
	,NOTSELECTED
}Choice;

/*
Description: Function display hompage to user and return his choise.
Input: Void.
Output: User choise.
Complexity: o(1).
*/
Choice ClientGUI_Homepage(void);

/*
Description: Function get username and password from user.
Input: Char pointer of username and password.
Output: void.
Complexity: o(1).
*/
void ClientGUI_GetUserDetails(char* _username, char* _password);
/*
Description: Function display hompage to user and return his choise.
Input: Void.
Output: User choise.
Complexity: o(1).
*/
Choice ClientGUI_GroupMenu(void);
/*
Description: Function print the command clear.
Input: Void.
Output: void.
Complexity: o(1).
*/
void UICleanScreen();

#endif /*CLIENT_GUI_H*/
