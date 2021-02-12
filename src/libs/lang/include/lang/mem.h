#if !defined(LANG_MEM_H)
#define LANG_MEM_H

#include "except.h"

/* GLOBAL declaration */
extern const Except_T Mem_Failed;

/**
 * Allocates `nbytes` bytes and returns a pointer to the first byte. The
 * bytes are uninitialized. If `Mem_alloc` raises `Mem_Failed`, file and
 * line are reported as the offending source coordinates.
 *
 * @throw    Mem_Failed
 */
extern void* Mem_alloc(long nbytes, const char* file, int line);
/**
 * Allocates space for an array of count elements, each occupying `nbytes` bytes,
 * and returns a pointer to the first element. It is a run-time error for
 * `count <= 0`. The elements are cleared, which does not necessarily initialize
 * pointers to NULL or floating-point values to 0.0. If `Mem_calloc` raises
 * `Mem_Failed`, file and line are reported as the offending source coordinate.
 *
 * @throw    Mem_Failed
 */
extern void* Mem_calloc(long count, long nbytes, const char* file, int line);

/**
 * De-allocates `ptr`, if `ptr` is non NULL. It is a run-time error for `ptr` to
 * be a pointer that was not returned by a previous call to a `Mem_alloc` function.
 * Implementations may use file and line to report memory-usage errors.
 *
 * @throw    Mem_Failed
 */
extern void  Mem_free(void* ptr, const char* file, int line);
/**
 *
 *
 * @throw    Mem_Failed
 */
extern void* Mem_resize(void* ptr, long nbytes, const char* file, int line);

#define ALLOC(nbytes)          Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes)  Mem_calloc((count), (nbytes), __FILE__, __LINE__)

#define NEW(p)    ((p) = ALLOC( (long)sizeof * (p)) )
#define NEW_0(p)  ((p) = CALLOC(1, (long)sizeof * (p)) )

#define FREE(ptr)            ((void)(Mem_free((ptr), __FILE__, __LINE__), (ptr) = 0))
#define RESIZE(ptr, nbytes)  ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))

#endif  /* LANG_MEM_H */
