#if !defined(LANG_MACROS_H)
#define LANG_MACROS_H

#include <stddef.h>

/*
 * Placed at the beginning of a function it suppress compiler
 * warnings about unused parameters.
 */
#define UNUSED(param_name)                                    \
  ((void)(0 ? ((param_name) = (param_name)) : (param_name)))

/* LOCAL void foo (void); makes clear that 'foo' is local in its module */
#define LOCAL   static

/* Makes clear that is a huge number */
#define INT64   long long
#define UINT64  unsigned long long

#define INT_TO_PTR(i)  ( (void*)(uintptr_t)(i) )
#define PTR_TO_INT(p)  ( (int)(uintptr_t)(p) )

#define ARRAY_SIZE(a)  ((sizeof(a)/sizeof(0[a])) / ((size_t)(!(sizeof(a) % sizeof(0[a])))))

#define CONTAINER_OF(ptr, type, member)  ((type *)(((char *)ptr) - offsetof(type, member)))

#endif  /* LANG_MACROS_H */
