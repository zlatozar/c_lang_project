/**
 * @file     arena.h
 *
 * Memory-management interface and an implementation that uses arena-based
 * algorithms, which allocate memory from an arena and deallocate entire arenas
 * at once.
 */
#if !defined(LANG_ARENA_H)
#define LANG_ARENA_H

#include <stddef.h>
#include "except.h"

typedef struct arena* Arena_T;

/* Checked exceptions */
extern const Except_T Arena_NewFailed;
extern const Except_T Arena_Failed;

/**
 * Allocates, initializes, and returns a new arena.
 *
 * @throw  `Arena_NewFailed` if can't allocate memory.
 */
extern Arena_T  Arena_new(void);

/**
 * Allocates `nbytes` bytes in arena and returns a pointer to the first byte.
 * The bytes are uninitialized. If `Arena_alloc` raises `Arena_Failed`, file and
 * line are reported as the offending source coordinates.
 *
 * @throw  `Arena_Failed` if can't allocates memory.
 */
extern void*  Arena_alloc (Arena_T self, size_t nbytes, const char* file, int line);

/**
 * Allocates space in arena for an array of count elements, each occupying
 * `nbytes`, and returns a pointer to the first element. It is a checked runtime
 * error for `count <= 0`. The elements are uninitialized. If `Arena_calloc`
 * raises `Arena_Failed`, file and line are reported as the offending source
 * coordinates.
 *
 * @throw  `Arena_Failed` if can't allocates memory.
 */
extern void*  Arena_calloc(Arena_T self, size_t count, size_t nbytes, const char* file, int line);

/**
 * Deallocates all of the space in *ap, deallocates the arena itself, and clears
 * `*p_self`. It is a checked runtime error for `p_self` or `*p_self` to be null.
 */
extern void   Arena_dispose(Arena_T* p_self);

/**
 * Deallocates all of the space in arena — all of the space allocated since
 * the last call to `Arena_free`.
 */
extern void   Arena_free(Arena_T self);

#endif  /* LANG_ARENA_H */
