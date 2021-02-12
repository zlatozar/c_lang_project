#if !defined(LANG_MACROS_H)
#define LANG_MACROS_H

#include <stddef.h>

/*
 * Placed at the beginning of a function it suppress compiler
 * warnings about unused parameters.
 */
#define UNUSED(param_name)                                 \
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

// _____________________________________________________________________________
//                                                                         Bits

#define BIT(x)           (1 << (x))
#define SETBIT(x, p)     ((x) | (1 << (p)))
#define CLEARBIT(x, p)   ((x) & (~(1 << (p))))
#define GETBIT(x, p)     (((x) >> (p)) & 1)
#define TOGGLEBIT(x, p)  ((x) ^ (1 << (p)))
#define LSB(x)           ((x) ^ ((x) - 1) & (x))

// _____________________________________________________________________________
//                                                                       String

#define STR_ALLOC(str)          ( (char *)malloc(strlen(str) + 1) )
#define STR_CONCAT(str1, str2)  (str1 "" str2)
#define STR_EQ(p, q)            (((*p)==(*q)) && (strcmp((p),(q)) == 0))

#endif  /* LANG_MACROS_H */
