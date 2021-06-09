/** Description:
*** The Implementation of HashMap includes an array of Pair*
*** Pair includes a void* key and a void* value
*** The array size is decided by the module SIZE_AFTER_LF and the next prime after that number
*** The method is open addressing with linear probing
*** the index into the item should be enter is decided by the hash function modulo the size of the array
***/

#include "HashMap.h"
#include <stdlib.h>


typedef struct Pair
{
	void* m_key;
	void* m_value;
} Pair;

struct HashMap
{
	Pair** m_arr;
	size_t m_size;
	size_t (*m_HashFunction)(const void* _key);
	int (*m_KeyEqualityFunction)(const void* _firstKey, const void* _secondKey);
	size_t m_num_items;
};

static int deleted = -1; 

#define ARR(map)	((map)->m_arr)
#define SIZE(map)	((map)->m_size)
#define FUNC(map)	((map)->m_HashFunction)
#define EQUAL(map)	((map)->m_KeyEqualityFunction)
#define NUM_ITEMS(map)		((map)->m_num_items)
#define KEY(pair)	((pair)->m_key)
#define VALUE(pair)	((pair)->m_value)

#define INDEX(map,key)		((FUNC(map))(key)%SIZE(map))
#define SIZE_AFTER_LF(size)	((int)((size)*1.3))

#define DELETED ((Pair*)&(deleted))
#define NOT_FOUND -1
#define FALSE 0
#define TRUE !FALSE

static size_t NextIndex(HashMap* map, size_t index)
{
	return (index+1)%SIZE(map);
}
static size_t isPrime(int n)  
{  
     int i;
     
    if (n <= 1)  return FALSE;  
    if (n <= 3)  return TRUE;  
    
    
    if (n%2 == 0 || n%3 == 0) return FALSE;  
    
    for (i=5; i*i<=n; i=i+6)  
        if (n%i == 0 || n%(i+2) == 0)  
           return FALSE;  
    
    return TRUE;  
}  

static size_t NextPrime(int N) 
{ 
  	int prime;
  	size_t found;
  	
    
    if (N <= 1) 
        return 2; 
  
    prime = N; 
    found = FALSE; 
  
    while (!found) { 
        prime++; 
  
        if (isPrime(prime)) 
            found = TRUE; 
    } 
  
    return prime; 
} 

static size_t IsKeyThere(HashMap* _map,size_t _index, const void* _key)
{
	if (ARR(_map)[_index] != NULL && ARR(_map)[_index] != DELETED)
	{
		return EQUAL(_map)(_key, KEY(ARR(_map)[_index]));
	}
	return FALSE;
}

static int GetIndexByKey(HashMap* _map, const void* _key)
{
	size_t index;
	int count = 0;
	
	index = INDEX(_map,_key);
	
	while (ARR(_map)[index] !=NULL && !IsKeyThere(_map,index,_key) && (++count != SIZE(_map))) 
	{
		index = NextIndex(_map, index);
	}
	
	if (ARR(_map)[index] ==NULL || count == SIZE(_map))
	{
		return NOT_FOUND;
	}
	
	return index;
}
	
static Pair* PairCreate(void* _key , void* _value)
{
	Pair* pair;
	
	if ((pair = (Pair*)malloc(sizeof(Pair))) == NULL)
	{
		return NULL;
	}
	
	KEY(pair) = _key;
	VALUE(pair) = _value;
	
	return pair;
}

HashMap* HashMap_Create(size_t _capacity, HashFunction _hashFunc, KeyEqualityFunction _keysEqualFunc)
{
	HashMap* map;
	int size;
	
	if (_hashFunc==NULL || _keysEqualFunc==NULL)
	{
		return NULL;
	}
	
	if ((map = (HashMap*)malloc(sizeof(HashMap)))==NULL)
	{
		return NULL;
	}
	
	size = NextPrime(SIZE_AFTER_LF(_capacity)); 
	if ((ARR(map) = (Pair**)calloc(size, sizeof(Pair*)))==NULL)
	{
		free(map);
		return NULL;
	}
	
	SIZE(map) = size;
	FUNC(map) = _hashFunc;
	EQUAL(map) = _keysEqualFunc;
	NUM_ITEMS(map) = 0;
	
	return map;
}

Map_Result HashMap_Insert(HashMap* _map, const void* _key, const void* _value)
{
	size_t index;
	
	if (_map==NULL)
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	if (_key==NULL)
	{
		return MAP_KEY_NULL_ERROR;
	}
	
	if (NUM_ITEMS(_map) == SIZE(_map))
	{
		return MAP_OVERFLOW_ERROR;
	}
	
	index = INDEX(_map,_key);
	
	while (ARR(_map)[index]!=NULL && ARR(_map)[index] != DELETED && !IsKeyThere(_map,index,_key)) 
	{
		index = NextIndex(_map, index);
	}
	
	if (IsKeyThere(_map,index,_key))
	{
		return MAP_KEY_DUPLICATE_ERROR;
	}
	
	ARR(_map)[index] = PairCreate((void*)_key, (void*)_value); 
	if (ARR(_map)[index]==NULL)
	{
		return MAP_ALLOCATION_ERROR;
	}
	
	NUM_ITEMS(_map)++;
	
	return MAP_SUCCESS;
}
	
Map_Result HashMap_Find(const HashMap* _map, const void* _searchKey, void** _pValue)
{
	size_t index;
	
	if (_map==NULL)
	{
		return MAP_UNINITIALIZED_ERROR;
	}
	if (_searchKey==NULL)
	{
		return MAP_KEY_NULL_ERROR;
	}
	
	if ((index = GetIndexByKey((HashMap*)_map,_searchKey))==NOT_FOUND)
	{
		return MAP_KEY_NOT_FOUND_ERROR;
	}
	
	if (_pValue != NULL)
	{
		*_pValue = VALUE(ARR(_map)[index]); 
	}
	
	return MAP_SUCCESS;
}

Map_Result HashMap_Remove(HashMap* _map, const void* _searchKey, void** _pKey, void** _pValue)
{
	size_t index;
	Map_Result res;
	
	if ((res = HashMap_Find(_map,_searchKey,_pValue))!= MAP_SUCCESS)
	{
		return res;
	}
	
	index = GetIndexByKey(_map,_searchKey);
	
	if (_pKey != NULL)
	{
		*_pKey = KEY(ARR(_map)[index]); 
	}
	
	free(ARR(_map)[index]);
	ARR(_map)[index] = DELETED;
	
	NUM_ITEMS(_map)--;
	
	return MAP_SUCCESS;
}

size_t HashMap_Size(const HashMap* _map)
{
	if (_map!=NULL)
	{
		return NUM_ITEMS(_map);
	}
	
	return 0;
}
	
size_t HashMap_ForEach(const HashMap* _map, KeyValueActionFunction _action, void* _context)
{
	size_t count=0;
	int i;
	
	for (i=0; i<SIZE(_map); i++)
	{
		if (ARR(_map)[i] != NULL && ARR(_map)[i] != DELETED)
		{
			count++;
			if (!_action(KEY(ARR(_map)[i]), VALUE(ARR(_map)[i]), _context))
			{
				break;
			}
		}
	}
	
	return count;
}
	
size_t HashMap_HashSize(const HashMap* _map)
{
	if (_map!=NULL)
	{
		return SIZE(_map);
	}
	return 0;
}


void HashMap_Destroy(HashMap** _map, void (*_keyDestroy)(void* _key), void (*_valDestroy)(void* _value))
{
	int i;
	
	if (*_map ==NULL)
	{
		return;
	}
	
	for (i=0; i<SIZE(*_map); i++)
	{
		if (ARR(*_map)[i] != NULL && ARR(*_map)[i] != DELETED)
		{
			if (_keyDestroy != NULL)
			{
				_keyDestroy(KEY(ARR(*_map)[i]));
			}	
			if (_valDestroy != NULL)
			{
				_valDestroy(VALUE(ARR(*_map)[i]));
			}
			free(ARR(*_map)[i]);
		}
	}
	
	free(ARR(*_map));
	free(*_map);
	*_map = NULL;
}			

	
	
	
	
