/* $Id */

// Convention - DIRECTORY_NAME_H
#ifndef MACROS_LANG_H
#define MACROS_LANG_H


/* LOCAL void foo (void); makes clear that 'foo' is local in its module */
#define LOCAL   static

#define INT16   short
#define UINT16  unsigned short
#define INT32   long
#define INT64   long long
#define UINT64  unsigned long long

typedef enum { OK, ERROR } status;

typedef void* generic_ptr;

#endif /* MACROS_LANG_H */
