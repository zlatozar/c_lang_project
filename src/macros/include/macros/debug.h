/* $Id */

// Convention - DIRECTORY_NAME_H
#ifndef MACROS_DEBUG_H
#define MACROS_DEBUG_H


#define IS_ARRAY(a)      ((void *)&a == (void *)a)
#define MEMCMP(A, o, B)  (memcmp((A), (B)) o 0)

#define DEBUG(fmt, ...)                                          \
  do { if (DEBUG) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__,  \
                      __LINE__, __func__, __VA_ARGS__); } while (0)

/* Example: assert(IMPLIES(n > 0, array != NULL)); */
#define IMPLIES(x, y)    (!(x) || (y))

/*
 * Placed at the beginning of a function it suppress compiler
 * warnings about unused parameters.
 */
#define UNUSED(param_name)                                 \
  ((void)(0 ? ((param_name) = (param_name)) : (param_name)))


#endif /* MACROS_DEBUG_H */
