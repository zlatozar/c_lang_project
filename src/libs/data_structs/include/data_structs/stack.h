/**
 * @file    stack.h
 * @brief   Stack ADT interface.
 */
#if !defined(DATA_STRUCTS_STACK_H)
#define DATA_STRUCTS_STACK_H

#include <stddef.h>    /* size_t */
#include "lang/extend.h"

typedef struct {
  Generic_T* storage;
  Generic_T* top;      /* Points to next available to push */
  unsigned size;
} stack_t;

typedef stack_t* Stack_T;

/**
 * Allocating space for the stack and initialize it to empty.
 */
extern mem_status Stack_init(Stack_T stack);

/**
 * Return `true` if empty otherwise `false`.
 */
extern bool Stack_is_empty(Stack_T stack);

/**
 * Push data onto stack. If there is no more room in the stack, allocate a
 * larger chunk of memory.
 */
extern mem_status Stack_push(Stack_T self, Generic_T data);

/**
 * Pop the top value of the stack and put in `p_data__`.
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern void Stack_pop(Stack_T stack, Generic_T* p_data__);

/**
 * Return the value at the top of the stack without removing it.
 * It is checked runtime error if empty stack is passed.
 * Client code should be responsible to free poped data.
 */
extern void Stack_peel(Stack_T stack, Generic_T* p_data__);


#endif  /* DATA_STRUCTS_STACK_H */
