#define _POSIX_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include "HashMap.h"
#include "UsersTable.h"
#include "List.h"
#include "List_Itr.h"

#define CAPACITY 100
#define FILE_NAME "users"
#define LEN 50
#define PRIME 31

struct Table
{
	HashMap*	m_map;
	char**		m_allocated;
};

struct User
{
	char* m_password;
	int m_isActive;
};

static size_t Hash_Func(const void* _key)
{
	size_t hash = 0;
	int i;
	const char* word = _key;
	
    for (i = 0 ; word[i] != '\0' ; i++)
    {
        hash = PRIME*hash + word[i];
    }
    return hash;
}

static int IsEqual(const void* usr1, const void* usr2)
{
	return !strcmp((char*)usr1, (char*)usr2);
}

static void InitBuff(char* str)
{
	int i;
	for (i=0; i<LEN; i++)
	{
		str[i] = '\0';
	}
}

Table* UserTable_CreateTable()
{
	Table* table;
	User* user;
	FILE* file;
	char buff[LEN];
	int i, c, count = 0;
	char* usr, *pass;
	
	table = (Table*)malloc(sizeof(Table));
	if(table==NULL)
	{
		return NULL;
	}
	
	table->m_allocated = (char**)malloc(sizeof(char*) * CAPACITY * 2);
	if(table->m_allocated == NULL)
	{
		free(table);
		return NULL;		
	}
	
	table->m_map = HashMap_Create(CAPACITY, Hash_Func , IsEqual);
	if(table->m_map == NULL)
	{
		free(table->m_allocated);
		free(table);
		return NULL;
	}
	
	if((file = fopen(FILE_NAME, "r"))==NULL)
	{
		return table;
	}
	
	while((c = fgetc(file)) != EOF)
	{
		InitBuff(buff);
		i = 0;
		while(c != ' ')
		{
			buff[i++] = c;
			c = fgetc(file);
		}
		
		usr = (char*)malloc(strlen(buff)+1);
		if(usr == NULL)
		{
			UserTable_DestroyTable(table);
			return NULL;
		}
		strcpy(usr,buff);
		table->m_allocated[count++] = usr;
		
		InitBuff(buff);
		i = 0;
		c = fgetc(file);
		while(c != '\n')
		{
			buff[i++] = c;
			c = fgetc(file);
		}
		
		pass = (char*)malloc(strlen(buff)+1);
		if(pass == NULL)
		{
			UserTable_DestroyTable(table);
			return NULL;
		}
		strcpy(pass,buff);
		table->m_allocated[count++] = pass;
		
		user = (User*)malloc(sizeof(User));
		if (user == NULL)
		{
			UserTable_DestroyTable(table);
			return NULL;
		}
		user->m_password = (char*)malloc(strlen(buff)+1);
		strcpy(user->m_password, pass);
		user->m_isActive = 0;

		HashMap_Insert(table->m_map, usr, user);
	}
	
	fclose(file);

	return table;
}

Tbl_Res UserTable_AddUser(Table* _tbl, char* _username, char* _password)
{
	Map_Result res;
	FILE* file;
	User* user;
	
	user = (User*)malloc(sizeof(User));
	if (user == NULL)
	{
		return TBL_ERR;
	}
	user->m_password = _password;
	user->m_isActive = 0;

	if ((res = HashMap_Insert(_tbl->m_map, _username, user)) == MAP_KEY_DUPLICATE_ERROR)
	{
		return TBL_USER_EXIST;
	}
	
	if (res != MAP_SUCCESS)
	{
		return TBL_ERR;
	}
	
	if ((file = fopen(FILE_NAME, "a"))==NULL)
	{
		return TBL_ERR;
	}
	
	fputs(_username,file);
	fputc(' ',file);
	fputs(_password,file);
	fputc('\n',file);
	
	fclose(file);
	
	return TBL_OK;
}

Tbl_Res UserTable_IsMatch(Table* _tbl, char* _username, char* _password)
{
	Map_Result res;
	User* user;
	
	res = HashMap_Find(_tbl->m_map, _username, (void**)&user);
	
	if (res == MAP_KEY_NOT_FOUND_ERROR)
	{
		return TBL_USER_NOT_EXIST;
	}
	
	return ((strcmp(_password, ((User*)user)->m_password)) ? TBL_USER_NO_MATCH : TBL_OK);
}

Tbl_Res UserTable_IsUserExist(Table* _tbl, char* _username)
{
	Map_Result res;
	res = HashMap_Find(_tbl->m_map, _username, NULL);
	
	if(res == MAP_KEY_NOT_FOUND_ERROR)
	{
		return TBL_USER_NOT_EXIST;
	}
	
	return TBL_USER_EXIST;
}

void destroyUser(void* user)
{
	if (user)
	{
		free(((User*)user)->m_password);
		free(user);
	}
}

void DestroyName(void* _key)
{
	free(_key);
}

void UserTable_DestroyTable(Table* _tbl)
{
	int i = 0;
	HashMap_Destroy(&(_tbl->m_map), DestroyName, destroyUser);
	while(_tbl->m_allocated[i] != NULL)
	{
		free(_tbl->m_allocated[i]);
		i++;
	}
	free(_tbl->m_allocated);
	free(_tbl);
}

int UserTable_IsActive(Table* _tbl, char* _username)
{
	Map_Result res;
	User* user;
	
	res = HashMap_Find(_tbl->m_map, _username, (void**)&user);
	
	if (res == MAP_KEY_NOT_FOUND_ERROR)
	{
		return 0;
	}
	return ((User*)user)->m_isActive;
}

void UserTable_ActivationSet(Table* _tbl, char* _username, int _isActive)
{
	Map_Result res;
	User* user;
	
	res = HashMap_Find(_tbl->m_map, _username, (void**)&user);
	if (res == MAP_KEY_NOT_FOUND_ERROR)
	{
		return;
	}
	((User*)user)->m_isActive = _isActive;
}

	
	
	
