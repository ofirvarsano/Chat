#include "Groups.h"
#include "IPTable.h"
#include "HashMap.h"
#include <string.h>

#define SIZE 50
#define PRIME 31

struct Groups
{
	HashMap* m_table;
	IPPool* m_ips;
	int m_size;
};

typedef struct
{
	char* m_ipAdd;
	size_t m_groupSize;
}GroupValue;

size_t Hash_Func(const void* _key)
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

int IsEqual(const void* usr1, const void* usr2)
{
	return !strcmp((char*)usr1, (char*)usr2 );
}

void DestroyNameKey(void* _key)
{
	free(_key);
}

void DestroyVal(void* _value)
{
	free(_value);
}

Groups* GroupsCreate()
{
	Groups* groups;
	
	groups = (Groups*)malloc(sizeof(Groups));
	if (groups==NULL)
	{
		return NULL;
	}
	
	groups->m_ips = PoolCreate();
	if (NULL==groups->m_ips)
	{
		free(groups);
		return NULL;
	}
	
	groups->m_table = HashMap_Create(SIZE, Hash_Func, IsEqual);
	if (NULL==groups->m_table)
	{
		PoolDestroy(groups->m_ips);
		free(groups);
		return NULL;
	}
	groups->m_size = 0;
	
	return groups;
}

GroupsErr CreateGroup(Groups* groups, char* name, char* ip)
{
	GroupValue* val;
	int len;
	char* GroupName;
	
	if (IsGroupExist(groups,name))
	{
		return GRP_EXIST;
	}
	
	val = (GroupValue*)malloc(sizeof(GroupValue));
	if (val==NULL)
	{
		return MEMORY_FAIL;
	}
	
	len = strlen(name);
	GroupName = malloc(len+1);
	if (GroupName==NULL)
	{
		free(val);
		return MEMORY_FAIL;
	}
	strcpy(GroupName, name);
	
	val->m_groupSize = 0;
	PoolPullIP(groups->m_ips , &(val->m_ipAdd));
	
	HashMap_Insert(groups->m_table, GroupName, val);
	groups->m_size++;
	
	strcpy(ip,val->m_ipAdd);
	
	return OK;
}

int IsGroupExist(Groups* groups, char* name)
{
	GroupValue* val = NULL;
	
	return (HashMap_Find(groups->m_table, name, (void**)&val)==MAP_SUCCESS)? 1:0;
}

GroupsErr GroupsSize(Groups* groups, char* name, int* size)
{
	GroupValue* val;
	
	if (HashMap_Find(groups->m_table, name, (void**)&val)!=MAP_SUCCESS)
	{
		return GRP_NOT_EXIST;
	}
	
	*size = val->m_groupSize;
	
	return OK;
}

int GroupsNumber(Groups* groups)
{
	if(groups)
	{
		return groups->m_size;
	}
	return 0;
}

GroupsErr GroupsSAddParticipant(Groups* groups, char* name, int delta, char* ip)
{
	GroupValue* val;
	char* key;
	
	if (HashMap_Find(groups->m_table, name, (void**)&val)!=MAP_SUCCESS)
	{
		return GRP_NOT_EXIST;
	}
	
	(val->m_groupSize) += delta;
	strcpy(ip,val->m_ipAdd);
	
	if (val->m_groupSize == 0)
	{
		PoolReturn(groups->m_ips, val->m_ipAdd);
		HashMap_Remove(groups->m_table , name, (void**)&key, (void**)&val);
		DestroyNameKey(key);
		DestroyVal(val);
	}
	
	return OK;
}

void GroupsDestroy(Groups* groups)
{
	HashMap_Destroy(&(groups->m_table), DestroyNameKey, DestroyVal);
	PoolDestroy(groups->m_ips);
	free(groups);
}

size_t Groups_ForEach(const Groups* _groups, ActionFunction _action, void* _context)
{
	return HashMap_ForEach(_groups->m_table, _action, _context);
}

