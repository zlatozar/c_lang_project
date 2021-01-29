#ifndef COMMON_DATA_STRUCTS_LIST_H
#define COMMON_DATA_STRUCTS_LIST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

typedef struct list* List_T;

extern List_T List_cons(void* head, List_T this);
extern void   List_free(List_T* p_this);

extern void*  List_head(List_T this);
extern List_T List_tail(List_T this);

extern size_t List_length(List_T this);
extern void*  List_nth(List_T this, unsigned n);

extern void   List_drop(List_T* p_this, unsigned n);

extern List_T List_append(List_T this, List_T listT);
extern List_T List_filter(bool (*pred_fn)( const void* ), List_T this);
extern List_T List_filter2(bool (*pred_fn)( void*, const void* ),
                           void* arg, List_T this);

extern List_T List_map(void* (*map_fn)( void* ), List_T this);
extern List_T List_map2(void* (*map_fn)( void*, void* ), void* arg,
                        List_T this);

extern List_T List_copy(List_T this);

extern void   List_print(void (*print_fn)( const void* ), List_T this);

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif  /* COMMON_DATA_STRUCTS_LIST_H */
