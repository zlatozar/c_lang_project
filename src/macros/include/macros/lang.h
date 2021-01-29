#ifndef MACROS_LANG_H
#define MACROS_LANG_H

#include <stddef.h>

/* LOCAL void foo (void); makes clear that 'foo' is local in its module */
#define LOCAL   static

/* Makes clear that is a huge number */
#define INT64   long long
#define UINT64  unsigned long long

#define INT_TO_PTR(i)  ( (void*)(uintptr_t)(i) )
#define PTR_TO_INT(p)  ( (int)(uintptr_t)(p) )

#define ARRAY_SIZE(a)  ((sizeof(a)/sizeof(0[a])) / ((size_t)(!(sizeof(a) % sizeof(0[a])))))

#define CONTAINER_OF(ptr, type, member)  ((type *)(((char *)ptr) - offsetof(type, member)))

#endif  /* MACROS_LANG_H */
