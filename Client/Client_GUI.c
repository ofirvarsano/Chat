#include"Client_GUI.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CREATE	1
#define JOIN	2
#define LEAVE	3
#define LOGOUT	4

Choice ClientGUI_Homepage(void)
{
	Choice menu = NOTSELECTED;	
	UICleanScreen();
	do
	{
		printf("1) Register\n");
		printf("2) Sign in\n");
		printf("3) Log out\n");
		printf("0) Exit\n");

		scanf("%u", &menu);
		switch(menu)
		{
			case EXIT:
				return menu;
			
			case REGISTER:
				return menu;			 
		
			case SIGN_IN:
				return menu;
			
			case LEAVE:
				return LOG_OUT;
				
			default :
	   			printf("Invalid input, try again\n");
				break;			
 		}
	}while(1);
return NOTSELECTED;
}


void ClientGUI_GetUserDetails(char* _username, char* _password)
{
	while(1)
	{
		fflush(stdin);
		fflush(stdout);
		printf("Please enter Username (up to %d letter)\n", BUFFER_SIZE);	
		if( 0 == scanf("%s", _username))
		{
			printf("%s\n", "Invalid input");
			continue;			
		}
		if(strlen(_username) > BUFFER_SIZE)
		{
			printf("%s\n", "username too long");
			continue;
		}
		printf("You Selected the following username : %s\n", _username);
		break;		
	}
	
	while(1)
	{
		fflush(stdin);
		fflush(stdout);
		printf("Please enter password (up to %d letter)\n", BUFFER_SIZE);
		if( 0 == scanf("%s", _password) )
		{
			printf("%s\n", "Invalid input");
			continue;			
		}
		if(strlen(_password) > BUFFER_SIZE)
		{
			printf("%s\n", "password too long");
			continue;
		}
		printf("You Selected the following password : %s\n", _password);
		break;
	}
}

Choice ClientGUI_GroupMenu(void)
{
	Choice menu = NOTSELECTED;
	UICleanScreen(); 
	do
	{	
		printf("Wellcome to group menu!\nPlease Enter your Choise:\n\n");
        printf("1) Create Group\n");
        printf("2) Join Group\n");
        printf("3) Leave Group\n");
        printf("4) Log Out\n");
        printf("0) Exit\n");
		
		scanf("%u", &menu);
		switch(menu)
		{
			case CREATE:
				return CREATE_GROUP;
			
			case JOIN:
				return JOIN_GROUP;			 
		
			case LEAVE:
				return LEAVE_GROUP;
			
			case LOGOUT:
				return LOG_OUT;
				
			case EXIT:
				return EXIT;

			default :
	   			printf("%s\n", "Invalid input, try again");
				break;			
 		}
	}while(1);
return NOTSELECTED;
}

void UICleanScreen()
{
	system("clear");
}



