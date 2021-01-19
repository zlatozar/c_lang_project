#ifndef COMMON_DATA_STRUCTS_STACK_H
#define COMMON_DATA_STRUCTS_STACK_H

#include <stdbool.h>

typedef struct stack* Stack_T;

extern Stack_T Stack_new(void);
extern void    Stack_free(Stack_T* self);

extern bool    Stack_isEmpty(Stack_T self);

extern void    Stack_push(Stack_T self, void* x);
extern void*   Stack_pop(Stack_T self);

#endif  /* COMMON_DATA_STRUCTS_STACK_H */
