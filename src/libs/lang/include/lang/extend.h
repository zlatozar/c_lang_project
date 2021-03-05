/**
 * @file    extend.h
 * @brief   Kind of <stdlib.h> extensions
 */
#if !defined(LANG_EXTEND_H)
#define LANG_EXTEND_H

#include <stdbool.h> /* bool     */
#include <limits.h>  /* CHAR_BIT */

/**
 * To emphasize that we work with raw data.
 */
typedef char byte;

/**
 *          |-current ... |-(current + 1) ...
 *        __v_________    v
 *  T -> |char*|next*|--> ...
 *        -----------
 *          |
 *          v
 *         data
 *
 * In the base data structure we can move around with pointer to the data
 * because it is `char*` so pointer arithmetic is possible. Operation that
 * include knowing of particular data type is done from client(wrapped)
 * functions. Because this pattern is very common this alias is created.
 */
typedef byte* generic_ptr;

/* Function types */

/**
 * When working with `void*` data compare function should be passed in
 * some operations.
 */
typedef bool compare_data_FN(generic_ptr, generic_ptr);

/**
 * Proper deallocation depends on the precise interface provided by functions.
 * It is therefore vital to document not only the functionality of functions,
 * but also how they manage the store. Because we don't know what will be stored,
 * function interface is introduced.
 */
typedef void (*free_data_FN)(void*);

/**
 * In many cases only client knows how to print data stored in structures
 * without type information that's why additional functions with this knowledge
 * should be passed.
 */
typedef void print_data_FN(generic_ptr);

/**
 * @brief    Used to indicate if allocation de-allocation succeed.
 *
 * In the base structure, project initially allocates memory, then the next
 * abstraction use this allocated memory and do some logic - could allocate
 * more. Then next abstraction of abstraction and so on. Problem in one of this
 * layers cause cascade for problems up to the top. That's way it is very import
 * to track memory problems checking `mem_status` and do rearguard actions in
 * case of ERROR eg. free memory, log or graceful stop.
 * @see    `List` ADT as an example usage
 *
 * Another way of doing this is using exceptions.
 * @see    `mem.h`
 * @see    `except.h`
 */
typedef enum { OK, ERROR } mem_status;

/**
 * @brief    Indicate if function finished without something unusual.
 *
 * Sometimes we need to be sure if function finished successfully and if not
 * there is no need to continue we can't recover at this level. What was left
 * is to mark as FAIL and pass error to the next level. Another case is when
 * out parameters are passed - we have to know if they contain correct data.
 */
typedef enum { SUCC, FAIL } status;

#define bitsizeof(type)  ((CHAR_BIT) * sizeof(type))

#endif  /* LANG_EXTEND_H */
