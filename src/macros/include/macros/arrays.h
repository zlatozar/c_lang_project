/* $Id */

// Convention - DIRECTORY_NAME_H
#ifndef MACROS_ARRAYS_H
#define MACROS_ARRAYS_H


#define SWAP(T, x, y)    do { T tmp = (x); (x) = (y); (y) = tmp; } while(0)
#define ARRAY_SIZE(a)    ((sizeof(a)/sizeof(0[a])) / ((size_t)(!(sizeof(a) % sizeof(0[a])))))

#endif /* MACROS_ARRAYS_H */
