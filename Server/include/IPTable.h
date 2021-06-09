#ifndef __IP_TABLE_H__
#define __IP_TABLE_H__

typedef struct IPPool IPPool;

/*
Description: Function Create ip pool.
Input: void.
Output: NULL on error or IPPool structure on success.
Complexity: o(1).
*/
IPPool* PoolCreate();
/*
Description: Function pull ip from queue.
Input: pointer to IPPool structure and double char pointer to pulled ip.
Output: void.
Complexity: o(1).
*/
void PoolPullIP(IPPool* pool, char** buffer);
/*
Description: Function put ip at the end of the queue.
Input: pointer to IPPool structure and char pointer buffer.
Output: void.
Complexity: o(1).
*/
void PoolReturn(IPPool* pool, char* buffer);
/*
Description: Function destroy ip pool queue.
Input: pointer to IPPool structure.
Output: void.
Complexity: o(1).
*/
void PoolDestroy(IPPool* pool);

#endif
