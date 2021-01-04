#include "common/lang/assert.h"

const Except_T Assert_Failed = { "Assertion failed!" };

/* NOTE: Parenthesis suppress macro expansion */
void (Assert)(int e) {
  Assert(e);
}

void Fail() {
  Assert(0);
}
