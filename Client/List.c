/*
	description: this module is the implimitation of generic List API
	magic number is inside m_item of head Node	
*/
#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "List_Itr.h"


#define LIST_ALIVE ((void*)0xDEADBEEF)
#define LIST_DEAD  ((void*)-1)
#define MAGIC(L) ((L)->m_head.m_item)
#define IS_VALID_LIST(L)		((L) && LIST_ALIVE == MAGIC(L))
#define IS_INVALID_LIST(L)	(! IS_VALID_LIST(L))
#define IS_EMPTY_LIST(L) ((L)->m_tail.m_prev == &(L)->m_head)



typedef struct ListNode ListNode;
struct ListNode{
	ListNode*	m_next;
	ListNode*	m_prev;
	void*		m_item;
};

struct List{
	ListNode	m_head;
	ListNode	m_tail;
};


static void* ListNodeRemove(ListNode* _removeMe);
static ListNode* ListNodeInsertBefore(ListNode* _beforeMe, void* _item);

List* List_Create(void)
{
	List* list = (List*)calloc(1, sizeof(List));

	if(!list)
	{
		return NULL;
	}	

	list->m_head.m_next = &list->m_tail;
	list->m_tail.m_prev = &list->m_head;
	MAGIC(list) = LIST_ALIVE;
	
	return list;
}

void List_Destroy(List* *_list, void (*_elementDestroy)(void* _item))
{
	ListNode* listPtr = NULL;
	if (!_list || IS_INVALID_LIST(*_list))
	{
		return;
	}

	MAGIC(*_list) = LIST_DEAD;

	listPtr = (*_list)->m_head.m_next;
	while(!IS_EMPTY_LIST(*_list))
	{
		ListNode *temp = listPtr;
		if (_elementDestroy!=NULL)
		{
			_elementDestroy(listPtr->m_item);
		}
		listPtr = listPtr->m_next;
		ListNodeRemove(temp);
	}
	
	free(*_list);
	*_list = NULL;
}


List_Result List_Push_Head(List* _list, void* _item)
{
	ListNode* newNode = NULL;
	if(IS_INVALID_LIST(_list))
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	
	newNode = ListNodeInsertBefore(_list->m_head.m_next, _item);	
	
	return (newNode) ? LIST_SUCCESS : LIST_ALLOCATION_ERROR;
}

List_Result List_Push_Tail(List* _list, void* _item)
{
	ListNode* newNode = NULL;
	if(IS_INVALID_LIST(_list))
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	
	newNode = ListNodeInsertBefore(&_list->m_tail, _item);	
	
	return (newNode) ? LIST_SUCCESS : LIST_ALLOCATION_ERROR;
}

static ListNode* ListNodeInsertBefore(ListNode* _beforeMe, void* _item)
{
	ListNode *newNode = (ListNode*)malloc(sizeof(ListNode));
	if(NULL != newNode)	
	{
		newNode->m_item = _item;
		newNode->m_next = _beforeMe;
		newNode->m_prev = _beforeMe->m_prev;
		_beforeMe->m_prev->m_next = newNode;
		_beforeMe->m_prev = newNode;
	}
	return newNode;
}

static void* ListNodeRemove(ListNode* _nodeToRemove)
{
	void* item = _nodeToRemove->m_item;

	_nodeToRemove->m_prev->m_next = _nodeToRemove->m_next;
	_nodeToRemove->m_next->m_prev = _nodeToRemove->m_prev;
	free(_nodeToRemove);

	return item;
}

List_Result List_Pop_Head(List* _list, void* *_pItem)
{
	if(IS_INVALID_LIST(_list))
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	if(!_pItem)
	{
		return LIST_INVALID_ARGUMENT;
	}
	if(IS_EMPTY_LIST(_list))
	{
		return LIST_UNDERFLOW_ERROR;
	}
	
	*_pItem = ListNodeRemove(_list->m_head.m_next);

	return LIST_SUCCESS;	
}

List_Result List_Pop_Tail(List* _list, void* *_pItem)
{
	if(IS_INVALID_LIST(_list))
	{
		return LIST_UNINITIALIZED_ERROR;
	}
	if(!_pItem)
	{
		return LIST_INVALID_ARGUMENT;
	}
	if(IS_EMPTY_LIST(_list))
	{
		return LIST_UNDERFLOW_ERROR;
	}
	
	*_pItem = ListNodeRemove(_list->m_tail.m_prev);

	return LIST_SUCCESS;	
}

int List_IsEmpty(const List* _list)
{
	return IS_VALID_LIST(_list) && IS_EMPTY_LIST(_list);
}

size_t List_Size(const List* _list)
{
	size_t size = 0;
	ListNode* listPtr = NULL;
	if(IS_INVALID_LIST(_list))
	{
		return 0;
	}
	
	for(listPtr = _list->m_head.m_next;
		listPtr != &_list->m_tail;
		listPtr = listPtr->m_next)
	{
		size++;
	}

	return size;
}

size_t List_ForEach(List* _list, ListElementAction _action, void* _context)
{
	int count = 0;
	ListNode* listPtr = NULL;
	if(IS_INVALID_LIST(_list) || !_action)
	{
		return 0;
	}

	for(listPtr = _list->m_head.m_next;
		listPtr != &_list->m_tail;
		listPtr = listPtr->m_next)
	{
		if (_action(listPtr->m_item, _context))
		{
			break;
		}
		count++;
	}
	return count;
}

ListItr ListItr_Begin(const List* _list)
{
	if(IS_INVALID_LIST(_list))
	{
		return NULL;
	}
	return  (ListItr)_list->m_head.m_next;
}

ListItr ListItr_End(const List* _list)
{
	if(IS_INVALID_LIST(_list))
	{
		return NULL;
	}
	return  (ListItr)_list->m_tail.m_prev->m_next;
}

int ListItr_Equals(const ListItr _a, const ListItr _b)
{
	return (_a && _b) ? (_a == _b) : 0;
}

ListItr ListItr_Next(ListItr _itr)
{
	ListNode* temp;
	if(NULL == _itr)
	{
		return NULL;
	}

	temp = (ListNode*)_itr;
	if(NULL == temp->m_next)
	{
		return _itr;
	}
	temp = temp->m_next;
	return (ListItr)temp;
}

ListItr ListItr_Prev(ListItr _itr)
{
	ListNode* temp;
	if(NULL == _itr)
	{
		return NULL;
	}

	temp = (ListNode*)_itr;
	if(NULL == temp->m_prev->m_prev)
	{
		return _itr;
	}
	temp = temp->m_prev;
	return (ListItr)temp;
}

void* ListItr_Get(ListItr _itr)
{
	if(NULL == _itr)
	{
		return NULL;
	}

	return (NULL == ((ListNode*)_itr)->m_next) ? NULL : ((ListNode*)_itr)->m_item;
}

void* ListItr_Set(ListItr _itr, void* _element)
{
	void* item = NULL;
	if(NULL == _itr)
	{
		return NULL;
	}
	
	item = ((ListNode*)_itr)->m_item;
	((ListNode*)_itr)->m_item = _element;

	return item;	
}

ListItr ListItr_InsertBefore(ListItr _itr, void* _element)
{
	if(NULL == _itr)
	{
		return NULL;
	}	

	return (ListItr)ListNodeInsertBefore((ListNode*)_itr, _element);
}

void* ListItr_Remove(ListItr _itr)
{
	if(NULL == _itr || NULL == ((ListNode*)_itr)->m_next)
	{
		return NULL;
	}

	return ListNodeRemove((ListNode*)_itr);
}

