#ifndef MACROS_LANG_H
#define MACROS_LANG_H


/* LOCAL void foo (void); makes clear that 'foo' is local in its module */
#define LOCAL   static

/* Makes clear that is very big */
#define INT64   long long
#define UINT64  unsigned long long

#define INT_TO_PTR(I)  ( (void*)(uintptr_t)(I) )
#define PTR_TO_INT(P)  ( (int)(uintptr_t)(P) )

#define SWAP(T, x, y)  do { T tmp = (x); (x) = (y); (y) = tmp; } while(0)
#define ARRAY_SIZE(a)  ((sizeof(a)/sizeof(0[a])) / ((size_t)(!(sizeof(a) % sizeof(0[a])))))

#endif  /* MACROS_LANG_H */
