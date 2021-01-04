#ifndef COMMON_LANG_ASSERT_H
#define COMMON_LANG_ASSERT_H

#ifdef  NDEBUG
#define Assert(e) ((void)0)

#else

#include "common/lang/except.h"

extern void Assert(int e);
extern void Fail();

#define Assert(e) ( (void)((e) || (RAISE(Assert_Failed), 0)) )

#endif  /* NDEBUG */
#endif  /* COMMON_LANG_ASSERT_H */
