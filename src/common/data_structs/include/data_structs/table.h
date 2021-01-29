#ifndef COMMON_DATA_STRUCTS_TABLE_H
#define COMMON_DATA_STRUCTS_TABLE_H

#include <stddef.h>  /* size_t */

typedef struct table* Table_T;

extern Table_T Table_new(unsigned hint, int cmp( const void* x, const void* y ),
                         unsigned hash( const void* key ));
extern void    Table_free(Table_T* self);

extern size_t  Table_length(Table_T self);

extern void*   Table_put(Table_T self, const void* key, void* value);
extern void*   Table_get(Table_T self, const void* key);
extern void*   Table_remove(Table_T self, const void* key);

extern void    Table_map(Table_T self, void apply( const void* key,
                                                   void** value, void* cl ),
                         void* cl);

extern void**  Table_toArray(Table_T self, void* end);

#endif  /* COMMON_DATA_STRUCTS_TABLE_H */
