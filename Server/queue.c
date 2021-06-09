#include <stdlib.h>
#include "queue.h"

#define QUEUE_ALIVE ((void*)0xDEADBEEF)
#define QUEUE_DEAD  ((void*)-1)

struct Queue
{
	void*  m_magic;
	size_t m_capacity;
	size_t m_head;
	size_t m_tail;
	void*  m_arr[1];
};



#define ARR(qu) ((qu)->m_arr)
#define SIZE(qu) ((qu)->m_capacity)
#define HEAD(qu) ((qu)->m_head)
#define TAIL(qu) ((qu)->m_tail)
#define MAGIC(qu) ((qu)->m_magic)

#define IS_VALID(Q)		((Q) && QUEUE_ALIVE == MAGIC(Q))
#define IS_INVALID(Q)	(! IS_VALID(Q))

#define CHECK_VALID(Q,E) do { if (IS_INVALID(Q)) return (E);} while(0)



#define NEXT(Q,I) (((I)+1) % (SIZE(Q)+1))
#define ITEM(Q,I) (ARR(Q)[(I)])

#define IS_EMPTY(q) (TAIL(q)==HEAD(q))
#define IS_FULL(q) (NEXT(q,TAIL(q))==HEAD(q))

#define CHECK_FULL(Q,E) do { if (IS_FULL(Q)) return (E); } while(0)
#define CHECK_EMPTY(Q,E) do { if (IS_EMPTY(Q)) return (E); } while(0)

Queue* Queue_Create(size_t _maxSize)
{
	Queue* queue;

	/* verify input arguments */
	if (_maxSize==0) {
		return NULL;
	} 

	/* allocate necessary storage */
	queue = (Queue*)malloc(sizeof(Queue) + sizeof(void*) * _maxSize);
	if (NULL == queue) {
		return NULL;
	}

	/* initialize state */
	SIZE(queue) = _maxSize;
	HEAD(queue) = 0;
	TAIL(queue) = 0;

	MAGIC(queue) = QUEUE_ALIVE;
	return queue;
}

void Queue_Destroy(Queue* *_queue, void (*_elementDestroy)(void* _item))
{
	if (!_queue || IS_INVALID(*_queue))
		return;

	MAGIC(*_queue) = QUEUE_DEAD;

	if (_elementDestroy!=NULL)
	{
		int i;
		for (i = HEAD(*_queue); i!=TAIL(*_queue); i=NEXT(*_queue,i))
		{
			_elementDestroy(ITEM(*_queue,i));
		}
	}

	free(*_queue);
	*_queue = NULL;
}

Queue_Result Queue_Enqueue(Queue* _queue, void* _item)
{
	CHECK_VALID(_queue, QUEUE_UNINITIALIZED_ERROR);
	CHECK_FULL(_queue, QUEUE_OVERFLOW);

	ITEM(_queue, TAIL(_queue)) = _item;
	TAIL(_queue) = NEXT(_queue, TAIL(_queue));

	return QUEUE_SUCCESS;
}


Queue_Result Queue_Dequeue(Queue* _queue, void* *_pValue)
{
	CHECK_VALID(_queue, QUEUE_UNINITIALIZED_ERROR);
	CHECK_EMPTY(_queue, QUEUE_UNDERFLOW);
	
	if (_pValue!=NULL)
	{
		*_pValue = ITEM(_queue,HEAD(_queue));
	}
	HEAD(_queue) = NEXT(_queue,HEAD(_queue));
	
	return QUEUE_SUCCESS;
}

int Queue_IsEmpty(const Queue* _queue)
{
	return IS_INVALID(_queue) || IS_EMPTY(_queue) ;
}

int Queue_IsFull (const Queue* _queue)
{
	return IS_VALID(_queue) && IS_FULL(_queue);
}

size_t Queue_ForEach(const Queue* _queue, QueueElementAction _action, void* _context)
{
	int i;
	int count = 0;

	if (IS_INVALID(_queue) || !_action)
		return 0;

	for (count = 0, i = HEAD(_queue);
		i!=TAIL(_queue) && _action(ITEM(_queue,i),_context);
		i=NEXT(_queue,i), count++)
	{}
	
	return count;
}

size_t Queue_Capacity(const Queue* _queue)
{
	return IS_VALID(_queue) ? _queue->m_capacity : 0;
}

size_t Queue_Size(const Queue* _queue)
{
	return IS_VALID(_queue) ? (_queue->m_tail - _queue->m_head) : 0;
}
