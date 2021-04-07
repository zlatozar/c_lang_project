/**
 * @file    lstack.h
 * @brief   Stack ADT interface using list for implementation.
 */
#if !defined(DATA_STRUCTS_LSTACK_H)
#define DATA_STRUCTS_LSTACK_H

#include "lang/extend.h"

typedef struct stack* LStack_T;

/**
 * @brief    Allocating space for the stack and initialize it to empty.
 */
extern LStack_T LStack_new(void);

/**
 * @brief    Return `true` if empty otherwise `false`.
 */
extern bool LStack_is_empty(LStack_T stack);

/**
 * @brief    Push data onto stack.
 */
extern void LStack_push(LStack_T stack, Generic_T data);

/**
 * @brief    Pop the top value of the stack and put in `p_data__`.
 *
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern bool LStack_pop(LStack_T stack, Generic_T* p_data__);

/**
 * @brief    Return the value at the top of the stack without removing it.
 *
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern bool LStack_peel(LStack_T stack, Generic_T* p_data__);

/**
 * @brief    Destroy stack as free all left data.
 *
 * Use it if data stored in stack needs special way of freeing
 * otherwise use `LStack_free`.
 */
extern void LStack_destroy(LStack_T stack, free_data_FN free_data_fn);

/**
 * @brief    Free resources.
 *
 * It is good practice to destroy empty stack. If `stack` has left
 * elements warn message will be logged.
 */
extern void LStack_free(LStack_T stack);


#endif  /* DATA_STRUCTS_LSTACK_H */
