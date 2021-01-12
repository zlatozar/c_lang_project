#ifndef COMMON_LANG_MEM_H
#define COMMON_LANG_MEM_H

#include "except.h"

extern const Except_T Mem_Failed;

extern void* Mem_alloc(size_t nbytes, const char* file, unsigned line);
extern void* Mem_calloc(size_t count, size_t nbytes, const char* file, unsigned line);

extern void  Mem_free(void* ptr, const char* file, unsigned line);
extern void* Mem_resize(void* ptr, size_t nbytes,	const char* file, unsigned line);

#define ALLOC(nbytes)          Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes)  Mem_calloc((count), (nbytes), __FILE__, __LINE__)

#define NEW(p)    ((p) = ALLOC( (long)sizeof * (p)) )
#define NEW_0(p)  ((p) = CALLOC(1, (long)sizeof * (p)) )

#define FREE(ptr)            ((void)(Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
#define RESIZE(ptr, nbytes)  ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))

#endif  /* COMMON_LANG_MEM_H */
