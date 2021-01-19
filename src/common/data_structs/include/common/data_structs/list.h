#ifndef COMMON_DATA_STRUCTS_LIST_H
#define COMMON_DATA_STRUCTS_LIST_H

#include <stdbool.h>

typedef struct list* List_T;

extern List_T List_cons(void* head, List_T self);
extern void   List_free(List_T* p_self);

extern void*  List_head(List_T self);
extern List_T List_tail(List_T self);

extern size_t List_length(List_T self);
extern void*  List_nth(List_T self, unsigned n);

extern void   List_drop(List_T* p_self, unsigned n);

extern List_T List_append(List_T self, List_T listT);
extern List_T List_filter(bool (*pred_fn)( const void* ), List_T self);
extern List_T List_filter2(bool (*pred_fn)( void*, const void* ),
                           void* arg, List_T self);

extern List_T List_map(void* (*map_fn)( void* ), List_T self);
extern List_T List_map2(void* (*map_fn)( void*, void* ), void* arg, List_T self);

extern List_T List_copy(List_T self);

extern void   List_print(void (*print_fn)( const void* ), List_T self);

#endif  /* COMMON_DATA_STRUCTS_LIST_H */
