/**
 * @file    mem.h
 * @brief   Less prone memory management.
 *
 * ATTENTION: Use NEW etc. FREE consistently.
 *
 * Provides a set of macros and routines that are less prone to error and that
 * provide a few additional capabilities.
 *
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
 *   3. An exception is an error that may be rare and perhaps unexpected, but from
 * which recovery may be possible.
 */
#if !defined(LANG_MEM_H)
#define LANG_MEM_H

#include <stddef.h>
#include "except.h"

/* GLOBAL declaration */
extern const Except_T Memory_Failed;

/**
 * Allocates `nbytes` bytes and returns a pointer to the first byte. The
 * bytes are uninitialized. If `Memory_alloc` raises `Memory_Failed`, file and
 * line are reported as the offending source coordinates.
 *
 * @throw `Memory_Failed` if not succeed.
 */
extern void* Memory_alloc(size_t nbytes, const char* file, int line);

/**
 * Allocates space for an array of `count` elements, each occupying `nbytes`
 * bytes, and returns a pointer to the first element. It is a checked run-time
 * error for `count <= 0`. The elements are cleared, which does not necessarily
 * initialize pointers to NULL or floating-point values to 0.0. If `Memory_calloc`
 * raises `Memory_Failed`, file and line are reported as the offending source
 * coordinate.
 *
 * @throw `Memory_Failed` if not succeed.
 */
extern void* Memory_calloc(size_t count, size_t nbytes, const char* file, int line);

/**
 * De-allocates `ptr`, if `ptr` is non NULL. It is a unchecked run-time error
 * for `ptr` to be a pointer that was not returned by a previous call to a
 * `Memory_alloc` function. Implementations may use file and line to report
 * memory-usage errors.
 */
extern void  Memory_free(void* ptr, const char* file, int line);

/**
 * Changes the size of the block at `ptr` to hold `nbytes` bytes, and returns a
 * pointer to the first byte of the new block. If `nbytes` exceeds the size of
 * the original block, the excess bytes are uninitialized. If `nbytes` is less
 * than the size of the original block, only `nbytes` of its bytes appear in the
 * new block. If `Memory_resize` raises `Memory_Failed`, file and line are reported as
 * the offending source coordinates. It is a checked run-time error for
 * `ptr=NULL`, and it is a unchecked for `ptr` to be a pointer that was not
 * returned by a previous call to a `Memory_alloc` function.
 *
 * @throw `Memory_Failed` if not succeed.
 */
extern void* Memory_resize(void* ptr, size_t nbytes, const char* file, int line);

#define ALLOC(nbytes)          Memory_alloc((nbytes), __FILE__, __LINE__)
#define CALLOC(count, nbytes)  Memory_calloc((count), (nbytes), __FILE__, __LINE__)

#define NEW(ptr)    ( (ptr) = ALLOC(    (long)sizeof(*(ptr))) )
#define NEW_0(ptr)  ( (ptr) = CALLOC(1, (long)sizeof(*(ptr))) )

#define FREE(ptr)            ( (void)(Memory_free((ptr), __FILE__, __LINE__), (ptr) = NULL) )
#define RESIZE(ptr, nbytes)  ( (ptr) = Memory_resize((ptr), (nbytes), __FILE__, __LINE__)   )

#endif  /* LANG_MEM_H */
