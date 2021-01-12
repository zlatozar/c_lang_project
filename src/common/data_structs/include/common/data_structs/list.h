#ifndef COMMON_DATA_STRUCTS_LIST_H
#define COMMON_DATA_STRUCTS_LIST_H

#include <stdbool.h>

typedef struct List_T* List_T;

extern List_T List_cons(void* head, List_T tail);
extern void   List_free(List_T* list);

extern void*  List_head(List_T list);
extern List_T List_tail(List_T list);

extern size_t List_length(List_T list);
extern void*  List_nth(List_T list, unsigned n);

extern void   List_drop(List_T list, unsigned n);

extern List_T List_append(List_T xs, List_T ys);
extern List_T List_filter(bool (*pred)( const void* ), List_T xs);
extern List_T List_filter2(bool (*pred)( void*, const void* ), void* arg, List_T xs);

extern List_T List_map(void* (*f)( void* ), List_T xs);
extern List_T List_map2(void* (*f)( void*, void* ), void* arg, List_T xs);

extern List_T List_copy(List_T xs);

extern void   List_print(void (*print)( const void* ), List_T xs);

#endif  /* COMMON_DATA_STRUCTS_LIST_H */
