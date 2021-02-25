/**
 * @file    macros.h
 * @brief   Various macros definitions to easy day to day coding.
 */
#if !defined(LANG_MACROS_H)
#define LANG_MACROS_H

#include <stddef.h>  /* size_t, offsetof */
#include <limits.h>  /* CHAR_BIT         */

/*
 * Placed at the beginning of a function it suppress compiler
 * warnings about unused parameters.
 */
#define UNUSED(param_name)                                   \
  ( (void)(0 ? ((param_name) = (param_name)) : (param_name)) )

/* LOCAL void foo (void); makes clear that 'foo' is local in its module */
#define LOCAL   static

#define bitsizeof(type)  ((CHAR_BIT) * sizeof(type))

#define ARRAY_SIZE(a)                                                  \
  ( (sizeof(a)/sizeof(0[a])) / ((size_t)(!(sizeof(a) % sizeof(0[a])))) )

#define CONTAINER_OF(ptr, type, member)              \
  ( (type *)(((char *)ptr) - offsetof(type, member)) )

#define INT_TO_PTR(i)    ( (void*)(uintptr_t)(i) )
#define PTR_TO_INT(p)    ( (int)(uintptr_t)(p)   )

/* __________________________________________________________________________ */
/*                                                                      Bits  */

#define BOOL(v)            ( !(!(v)) )

/* v = <target variable>, bn = <bit number to act upon 0 (first bit) up to n - 1 (last bit)> */
#define BIT_GET(v, bn)     ( ((v) >> (bn)) & 1UL   )
#define BIT_SET(v, bn)     ( (v) |= (1UL << (bn))  )    /* var |= 1UL << n;     */
#define BIT_CLEAR(v, bn)   ( (v) &= ~(1UL << (bn)) )    /* var &= ~(1UL << n);  */
#define BIT_TOGGLE(v, bn)  ( (v) ^ (1UL << (bn))   )    /* var ^= 1UL << n;     */
#define BIT_FLIP(v, bn)    ( (v) ^= (1UL << (bn))  )
#define BIT_CHECK(v, bn)   ( !(!((v) & (1UL << (bn)))) )

/* Select least significant bit */
#define LSB(v)             ( (v) ^ ((v) - 1UL) & (v) )

/* x = <target variable>, y = mask */
#define BITMASK_SET(v, m)        ( (v) |= (m)    )
#define BITMASK_CLEAR(v, m)      ( (v) &= (~(m)) )
#define BITMASK_FLIP(v, m)       ( (v) ^= (m)    )
#define BITMASK_CHECK_ALL(v, m)  ( !(~(v) & (m)) )
#define BITMASK_CHECK_ANY(v, m)  ( ((v) & (m)) == (m) )

#endif  /* LANG_MACROS_H */
