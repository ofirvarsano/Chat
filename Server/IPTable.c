#include "IPTable.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INITIAL "224.0.0."
#define LEN_OF_NUM 3
#define SIZE 200
#define BUF_LEN 12

struct IPPool
{
	Queue* m_pool;
};

static void InitialWithIPs(Queue* pool)
{
	char buff[4];
	int i;
	char* value;
	
	for (i= 1; i <= SIZE; i++)
	{
		value = calloc(BUF_LEN,sizeof(char));

		strcpy(value,INITIAL);
		sprintf(buff,"%d",i);
		strcat(value,buff);
		Queue_Enqueue(pool,value);
	}
}
		
void Destroy(void* _item)
{
	free(_item);
}
	
IPPool* PoolCreate()
{
	IPPool* pool;
	
	pool = (IPPool*)malloc(sizeof(IPPool));
	if (pool==NULL)
	{
		return NULL;
	}
	
	pool->m_pool = Queue_Create(SIZE);
	if (NULL == pool->m_pool)
	{
		free(pool);
		return NULL;
	}
	
	InitialWithIPs(pool->m_pool);
	
	return pool;
}

void PoolPullIP(IPPool* pool, char** buffer)
{
	Queue_Dequeue(pool->m_pool, (void**)buffer);
}

void PoolReturn(IPPool* pool, char* buffer)
{
	Queue_Enqueue(pool->m_pool,buffer);
}

void PoolDestroy(IPPool* pool)
{
	Queue_Destroy(&(pool->m_pool),Destroy);
}
	
	
	
