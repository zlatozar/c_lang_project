#if !defined(LANG_ASSERT_H)
#define LANG_ASSERT_H

#include "except.h"

#if defined(NDEBUG)

#define Assert(exp) ((void)0)
#define Require(exp) ((void)0)
#define Ensure(exp) ((void)0)
#define Invariant(exp) ((void)0)

#else

/**
 * @brief    Ad hoc program logic check.
 *
 * Raises `Assert_Failed` if `exp` is zero. Syntactically, `Assert(exp)`
 * is an expression. If NDEBUG is defined when `assert.h` is included,
 * assertions are disabled.
*/
extern void Assert(int exp);
/**
 * @brief    Pre-condition check.
 *
 * @see Assert
 */
extern void Require(int exp);
/**
 * @brief    Post-condition check.
 *
 * @see Assert
 */
extern void Ensure(int exp);
/**
 * @brief    Data structure integrity check.
 *
 * @see Assert
 */
extern void Invariant(int exp);

#define Assert(exp)     ( (void)((exp) || (THROW(Assert_Failed), 0)) )
#define Require(exp)    ( (void)((exp) || (THROW(Precondition_Failed), 0)) )
#define Ensure(exp)     ( (void)((exp) || (THROW(Postcondition_Failed), 0)) )
#define Invariant(exp)  ( (void)((exp) || (THROW(Invariant_Error), 0)) )

#endif  /* NDEBUG */
#endif  /* LANG_ASSERT_H */
