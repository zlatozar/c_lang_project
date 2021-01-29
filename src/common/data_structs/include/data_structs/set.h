#ifndef COMMON_DATA_STRUCTS_SET_H
#define COMMON_DATA_STRUCTS_SET_H

#include <stddef.h>

typedef struct set* Set_T;

extern Set_T  Set_new(size_t hint, int cmp( const void* x, const void* y ),
                      size_t hash( const void* x ));
extern void   Set_free(Set_T* this);

extern size_t Set_length(Set_T this);

extern int    Set_member(Set_T this, const void* member);

extern void   Set_put(Set_T this, const void* member);
extern void*  Set_remove(Set_T this, const void* member);

extern void   Set_map(Set_T this, void apply( const void* member, void* cl ),
                      void* cl);

extern void** Set_toArray(Set_T this, void* end);

extern Set_T  Set_union(Set_T xset, Set_T yset);
extern Set_T  Set_inter(Set_T xset, Set_T yset);
extern Set_T  Set_minus(Set_T xset, Set_T yset);
extern Set_T  Set_diff(Set_T xset, Set_T yset);

#endif  /* COMMON_DATA_STRUCTS_SET_H */
