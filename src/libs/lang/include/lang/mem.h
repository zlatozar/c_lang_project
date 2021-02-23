#if !defined(LANG_MEM_H)
#define LANG_MEM_H

#include <stddef.h>
#include "except.h"

/**
 * Three kinds of errors occur in programs: user errors, runtime errors (checked
 * and unchecked), and exceptions.
 *   1. User errors are expected because they’re likely
 * to occur as the result of erroneous user input. Programs must plan for and deal
 * with such errors. Usually, functions that must cope with user errors return
 * error codes — the errors are a normal part of the computation.
 *   2a. The checked runtime errors are at the other end of the error spectrum. They
 * are not user errors. They are never expected and always indicate program bugs.
 * Usually pre-conditions are `assert` is used to avoid undifined behavior.
 *   2b. The unchecked errors can't be handled by `assert` checks to avoid them we
 * use external tools, tests and smart program design.
 *    3. An exception is an error that may be rare and perhaps unexpected, but from
 * which recovery may be possible.
 */

/* GLOBAL declaration */
extern const Except_T Mem_Failed;

/**
 * Allocates `nbytes` bytes and returns a pointer to the first byte. The
 * bytes are uninitialized. If `Mem_alloc` raises `Mem_Failed`, file and
 * line are reported as the offending source coordinates.
 *
 * @throw `Mem_Failed` if NDEBUG is not defined
 */
extern void* Mem_alloc(size_t nbytes, const char* file, int line);
/**
 * Allocates space for an array of `count` elements, each occupying `nbytes`
 * bytes, and returns a pointer to the first element. It is a checked run-time
 * error for `count <= 0`. The elements are cleared, which does not necessarily
 * initialize pointers to NULL or floating-point values to 0.0. If `Mem_calloc`
 * raises `Mem_Failed`, file and line are reported as the offending source
 * coordinate.
 *
 * @throw `Mem_Failed` if NDEBUG is not defined
 */
extern void* Mem_calloc(size_t count, size_t nbytes, const char* file, int line);

/**
 * De-allocates `ptr`, if `ptr` is non NULL. It is a unchecked run-time error
 * for `ptr` to be a pointer that was not returned by a previous call to a
 * `Mem_alloc` function. Implementations may use file and line to report
 * memory-usage errors.
 *
 * @throw `Mem_Failed` if NDEBUG is not defined
 */
extern void  Mem_free(void* ptr, const char* file, int line);

/**
 * Changes the size of the block at `ptr` to hold `nbytes` bytes, and returns a
 * pointer to the first byte of the new block. If `nbytes` exceeds the size of
 * the original block, the **excess bytes** are uninitialized. If `nbytes` is
 * less than the size of the original block, only `nbytes` of its bytes appear
 * in the new block. If `Mem_resize` raises `Mem_Failed`, file and line are
 * reported as the offending source coordinates. It is a checked run-time error
 * for `ptr=NULL`, and it is a unchecked for `ptr` to be a pointer that was not
 * returned by a previous call to a `Mem_alloc` function.
 *
 * @throw `Mem_Failed` if NDEBUG is not defined
 */
extern void* Mem_resize(void* ptr, size_t nbytes, const char* file, int line);

#define ALLOC(nbytes)          Mem_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes)  Mem_calloc((count), (nbytes), __FILE__, __LINE__)

#define NEW(p)    ((p) = ALLOC( (long)sizeof * (p)) )
#define NEW_0(p)  ((p) = CALLOC(1, (long)sizeof * (p)) )

#define FREE(ptr)            ((void)(Mem_free((ptr), __FILE__, __LINE__), (ptr) = NULL))
#define RESIZE(ptr, nbytes)  ((ptr) = Mem_resize((ptr), (nbytes), __FILE__, __LINE__))

#endif  /* LANG_MEM_H */
