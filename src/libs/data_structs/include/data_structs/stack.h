/**
 * @file    stack.h
 * @brief   Stack ADT interface.
 */
#if !defined(DATA_STRUCTS_STACK_H)
#define DATA_STRUCTS_STACK_H

#include "lang/extend.h"

typedef struct stack* Stack_T;

/**
 * @brief    Allocating space for the stack and initialize it to empty.
 */
extern Stack_T Stack_new(void);

/**
 * @brief    Return `true` if empty otherwise `false`.
 */
extern bool Stack_is_empty(Stack_T stack);

/**
 * @brief    Push data onto stack.
 *
 * If there is no more room in the stack, allocate a larger
 * chunk of memory.
 */
extern void Stack_push(Stack_T stack, Generic_T data);

/**
 * @brief    Pop the top value of the stack and put in `p_data__`.
 *
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern bool Stack_pop(Stack_T stack, Generic_T* p_data__);

/**
 * @brief    Return the value at the top of the stack without removing it.
 *
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern bool Stack_peel(Stack_T stack, Generic_T* p_data__);

/**
 * @brief    Destroy stack as free all left data.
 *
 * Use it if data stored in stack needs special way of freeing
 * otherwise use `Stack_free`.
 */
extern void Stack_destroy(Stack_T stack, free_data_FN free_data_fn);

/**
 * @brief    Free resources.
 *
 * It is good practice to destroy empty stack. If `stack` has left
 * elements warn message will be logged.
 */
extern void Stack_free(Stack_T stack);


#endif  /* DATA_STRUCTS_STACK_H */
