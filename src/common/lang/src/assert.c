#include "common/lang/assert.h"

const Except_T Assert_Failed = { "Assertion failed" };

const Except_T Precondition_Failed = { "Pre-condition failed" };
const Except_T Postcondition_Failed = { "Post-condition failed" };
const Except_T Invariant_Error = { "Invariant error" };

/* NOTE: Parenthesis suppress macro expansion */

void (Assert)(int exp)
{
  Assert(exp);
}

void Fail()
{
  Assert(0);
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
