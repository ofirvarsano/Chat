#ifndef GROUPS_H
#define GROUPS_H
#include <stdlib.h>

typedef struct Groups Groups;

typedef enum
{
	OK,
	MEMORY_FAIL,
	GRP_EXIST,
	GRP_NOT_EXIST
} GroupsErr ;

typedef int	(*ActionFunction)(const void* _key, void* _value, void* _context);

/*
Description: Function create structure for all groups.
Input: Void.
Output: The Groups structure.
Complexity: o(1).
*/
Groups* GroupsCreate();
/*
Description: Function create group.
Input: Groups structure, char pointer to name of group and ip.
Output: GRP_EXIST if group exist, MEMORY_FAIL if allocation failed and OK on success.
Complexity: o(1).
*/
GroupsErr CreateGroup(Groups* groups, char* name, char* ip);
/*
Description: Function check if group exist.
Input: Groups structure and char pointer to name.
Output: 1 if group exist and 0 if not.
Complexity: o(1).
*/
int IsGroupExist(Groups* groups, char* name);
/*
Description: Function get group size.
Input: Groups structure and char pointer to name and int pointer to size.
Output: GRP_NOT_EXIST if group not exist and OK on success.
Complexity: o(1).
*/
GroupsErr GroupsSize(Groups* groups, char* name, int* size);
/*
Description: Function add or remove a group member.
Input: Groups structure and char pointer to name and ip and integer number delta 1 to add and -1 to remove from group.
Output: GRP_NOT_EXIST if group not exist and OK on success.
Complexity: o(1).
*/
GroupsErr GroupsSAddParticipant(Groups* groups, char* name, int delta, char* ip);
/*
Description: Function destroy groups structure.
Input: Groups structure.
Output: void.
Complexity: o(1).
*/
void GroupsDestroy(Groups* groups);
/*
Description: Function return number of groups.
Input: Groups structure.
Output: number of groups.
Complexity: o(1).
*/
int GroupsNumber(Groups* groups);

size_t Groups_ForEach(const Groups* _groups, ActionFunction _action, void* _context);

#endif
