#include "lang/assert.h"

/* Initialize GLOBAL. Could be thrown from anywhere. */
const Except_T Assert_Failed = { "Assertion failed" };
const Except_T Precondition_Failed = { "Pre-condition failed" };
const Except_T Postcondition_Failed = { "Post-condition failed" };
const Except_T Invariant_Error = { "Invariant error" };

/* NOTE: Parenthesis suppress macro expansion */

void (Assert)(int exp)
{
  Assert(exp);
}

void (Require)(int exp)
{
  Require(exp);
}

void (Ensure)(int exp)
{
  Ensure(exp);
}

void (Invariant)(int exp)
{
  Invariant(exp);
}
