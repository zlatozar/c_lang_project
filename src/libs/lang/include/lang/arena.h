#if !defined(LANG_ARENA_H)
#define LANG_ARENA_H

#include <stddef.h>
#include "except.h"

typedef struct arena* Arena_T;

/* Checked exceptions */
extern const Except_T Arena_NewFailed;
extern const Except_T Arena_Failed;

extern Arena_T  Arena_new(void);

extern void*  Arena_alloc (Arena_T self, size_t nbytes, const char* file, int line);
extern void*  Arena_calloc(Arena_T self, size_t count, size_t nbytes, const char* file, int line);

extern void   Arena_dispose(Arena_T* p_self);
extern void   Arena_free   (Arena_T self);

#endif  /* LANG_ARENA_H */
