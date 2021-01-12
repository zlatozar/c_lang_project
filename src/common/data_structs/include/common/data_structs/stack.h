#ifndef COMMON_DATA_STRUCTS_STACK_H
#define COMMON_DATA_STRUCTS_STACK_H


typedef struct Stack_T* Stack_T;

extern Stack_T Stack_new(void);
extern void    Stack_free(Stack_T* stk);

extern int     Stack_empty(Stack_T stk);

extern void    Stack_push(Stack_T stk, void* x);
extern void*   Stack_pop(Stack_T stk);

#endif  /* COMMON_DATA_STRUCTS_STACK_H */
