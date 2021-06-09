#ifndef LIST_H
#define LIST_H
/**
 * @brief Create a Generic List data type
 * that stores pointer to user provided elements of generic type
 *
 */

#include <stddef.h>  /* size_t */

typedef struct List List;

typedef enum Queue_Result {
	 LIST_SUCCESS
	,LIST_UNINITIALIZED_ERROR						/**< Uninitialized list error 	*/
	,LIST_INVALID_ARGUMENT							/**< Invalud argument passed */
	,LIST_ALLOCATION_ERROR							/**< ListNode allocation failed */
	,LIST_UNDERFLOW_ERROR							/**< List is empty for remove operation */

	/* Add more as needed by your implementation */
} List_Result;

/**
 * @brief Dynamically create a new List object
 * @return List * - on success / NULL on fail
 *
 */
List* List_Create(void);

/**
 * @brief Dynamically deallocate a previously allocated list
 * @param[in] _list - List to be deallocated. Set to NULL following cleanup
 * @params[in] _elementDestroy : A function pointer to be used to destroy all
 * 					 remaining elements in the list or a null if no such destroy is required
 * @return void
 */
void List_Destroy(List** _list, void (*_elementDestroy)(void* _item));

/**
 * @brief push an Item to the head of the List.
 * @param[in] _list - List to operate on.
 * @param[in] _item - Item to be pushed.
 * @return success or error code
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if given uninitialized List
 * @retval LIST_ALLOCATION_ERROR if allocation for pushed item failed.
 * @warning _item can't be null. this will be assertion violation
 */
List_Result List_Push_Head(List* _list, void* _item);

/**
 * @brief push an Item to the tail of the List.
 * @param[in] _list - List to operate on.
 * @param[in] _item - Item to be pushed.
 * @return success or error code
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if given uninitialized List
 * @retval LIST_ALLOCATION_ERROR if allocation for pushed item failed.
 * @warning _item can't be null. this will be assertion violation
 */
List_Result List_Push_Tail(List* _list, void* _item);

/**
 * @brief pop an Item from the head of the List.
 * @param[in] _list - List to operate on.
 * @param[in] *_pItem - Item pointer to receive poped item.
 * @return success or error code
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if given uninitialized List
 * @warning _item can't be null. this will be assertion violation
 */
List_Result List_Pop_Head(List* _list, void* *_pItem);

/**
 * @brief pop an Item from the tail of the List.
 * @param[in] _list - List to operate on.
 * @param[in] *_pItem - Item pointer to receive poped item.
 * @return success or error code
 * @retval LIST_SUCCESS on success
 * @retval LIST_UNINITIALIZED_ERROR if given uninitialized List
 * @warning _item can't be null. this will be assertion violation
 */
List_Result List_Pop_Tail(List* _list, void* *_pItem);

/**
 * @brief Return true if the list is empty.
 * @param[in] _list - List to check.
 * @return  1 if empty 0 otherwise
 */
int List_IsEmpty(const List* _list);

/**
 * @brief Return size of given list.
 * @param[in] _list - List to check.
 * @return  size_t size of List, 0 if list is empty or otherwise.
 */
size_t List_Size(const List* _list);

#ifndef DEBUG
/**
 * The following are convenience function(s) for debugging purposes
 */
/**
 * @brief Iterate over all elements in the list.
 * @details The user provided _action function will be called for each element
 *          if _action returns a zero for an element the iteration will stop.
 * @param[in] _list  - list to iterate over.
 * @param[in] _action - User provided function pointer to be invoked for each element
 * @param[in] _context - User provided context, will be sent to _action
 * @returns number of times the user function was invoked
 *
 * Example use: List_ForEach(list, PrintElement, NULL)
 */

typedef int	(*ListElementAction)(void* _element, void* _context);
size_t List_ForEach(List* _list, ListElementAction _action, void* _context);

#endif

#endif /* LIST_H */
