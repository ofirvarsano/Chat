#include "genericVector.h"
#include <stdlib.h> /* malloc, free, size_t */

#define MAGIC 45678

struct Vector
{
	void** m_items;
	size_t m_originalSize;
	size_t m_nItems;
	size_t m_capacity;
	size_t m_blockSize;
	int m_magic;	
};

static int IsResizeNeeded(Vector* _vector);
static void** ResizeArray(void** _items, size_t _newSize);
static void ChangeSize(Vector* _vector);

Vector* Vector_Create(size_t _initialCapacity, size_t _blockSize)
{
	Vector* newVec = NULL;
	void** newData = NULL;
	if (_initialCapacity == 0 && _blockSize == 0)
	{
		return NULL;
	}
	newVec = (Vector*)malloc(sizeof(Vector));
	if (NULL == newVec)
	{
		return newVec;
	}
	newData = (void**)malloc(_initialCapacity * sizeof(void*));
	if (NULL == newData)
	{
		free(newVec);
		return NULL;
	}
	newVec->m_originalSize = _initialCapacity;
	newVec->m_capacity = _initialCapacity;
	newVec->m_nItems = 0;
	newVec->m_blockSize = _blockSize;
	newVec->m_items = newData;
	newVec->m_magic = MAGIC;

	return newVec;
}

void Vector_Destroy(Vector** _vector, void(*_elementDestroy)(void* _item))
{
	size_t i;
	if (_vector == NULL || *_vector == NULL || (*_vector)->m_magic != MAGIC)
	{
		return;
	}
	if ((*_vector)->m_items)
	{
		if (_elementDestroy != NULL)
		{
			for (i = 0; i < (*_vector)->m_nItems; ++i)
			{
				_elementDestroy((void*)((*_vector)->m_items[i]));
			}
		}
		free((*_vector)->m_items);
	}
	(*_vector)->m_magic = 0;
	free(*_vector);
	*_vector = NULL;
}

/* TODO delete it */
static void** ResizeArray(void** _items, size_t _newSize)
{
	return realloc(_items, _newSize * sizeof(void*));;
}

Vector_Result Vector_Append(Vector* _vector, void* _item)
{
	void** newItems = NULL;
	if (_vector == NULL || _vector->m_magic != MAGIC|| _vector->m_items == NULL || _item == NULL)
	{
		return VECTOR_UNITIALIZED_ERROR;
	}
	if (_vector->m_nItems >= _vector->m_capacity)
	{
		if(_vector->m_blockSize == 0)
		{
			return VECTOR_OVERFLOW_ERROR;
		}
		newItems = ResizeArray(_vector->m_items,_vector->m_capacity + _vector->m_blockSize);
		if(NULL == newItems)
		{
			return VECTOR_ALLOCATION_ERROR;
		}
		_vector->m_items = newItems;
		_vector->m_capacity += _vector->m_blockSize;
	}
	_vector->m_items[_vector->m_nItems] = _item;
	++_vector->m_nItems;
	return VECTOR_SUCCESS;
}

Vector_Result Vector_Remove(Vector* _vector, void** _pValue)
{
	if (_vector && _vector->m_items && _pValue)
	{
		if (_vector->m_nItems >= 1)
		{
			--_vector->m_nItems;
			*_pValue = (_vector->m_items[_vector->m_nItems]);
			
			if (IsResizeNeeded(_vector))
			{
				ChangeSize(_vector);
			}
			return VECTOR_SUCCESS;
		}
		return VECTOR_UNDERFLOW_ERROR;
	}
	return VECTOR_UNITIALIZED_ERROR;
}

static void ChangeSize(Vector* _vector)
{
	if (_vector)
	{
		void** newItems = NULL;
		size_t newSize;
		
		newSize = _vector->m_nItems + _vector->m_blockSize;
		newItems = ResizeArray(_vector->m_items,newSize);
		if (newItems)
		{
			_vector->m_items = newItems;
			_vector->m_capacity = newSize;
		}
	}
}

static int IsResizeNeeded(Vector* _vector)
{
	if (_vector->m_capacity > _vector->m_originalSize + _vector->m_blockSize && _vector->m_capacity > _vector->m_nItems + _vector->m_blockSize)
	{
		return 1;
	}
	return 0;
}

Vector_Result Vector_Get(const Vector* _vector, size_t _index, void** _pValue)
{
	if (_vector && _vector->m_items && _pValue)
	{
		if (_index >= 0 && _index < _vector->m_nItems)
		{
			*_pValue = (void*)(_vector->m_items[_index]);
			return VECTOR_SUCCESS;
		}
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}
	return VECTOR_UNITIALIZED_ERROR;
}

Vector_Result Vector_Set(Vector* _vector, size_t _index, void* _value)
{
	if (_vector && _vector->m_items && _value)
	{
		if (_index >= 0 && _index < _vector->m_nItems)
		{
			_vector->m_items[_index] = _value;
			return VECTOR_SUCCESS;
		}
		return VECTOR_INDEX_OUT_OF_BOUNDS_ERROR;
	}
	return VECTOR_UNITIALIZED_ERROR;
}

size_t Vector_Size(const Vector* _vector)
{
	if (_vector && _vector->m_items)
	{
		return _vector->m_nItems;
	}
	return 0;
}

size_t Vector_Capacity(const Vector* _vector)
{
	if (_vector && _vector->m_items)
	{
		return _vector->m_capacity;
	}
	return 0;
}

size_t Vector_ForEach(const Vector* _vector, VectorElementAction _action, void* _context)
{
	size_t i;
	if (_vector)
	{
		if (_action)
		{
			for (i = 0; i < Vector_Size(_vector); ++i)
			{
             	if(_action(_vector->m_items[i], i, _context) == 0)
             	{
					break;
				}	
			}
			return i;
		}
	}
	return 0;
}


