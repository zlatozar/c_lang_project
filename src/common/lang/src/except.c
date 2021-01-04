#include <stdlib.h>
#include <stdio.h>

#include "common/lang/assert.h"
#include "common/lang/except.h"

Except_Frame *Except_stack = NULL;

void Except_raise(const Except_T *e, const char *file,	int line) {

  Except_Frame *p = Except_stack;

  Assert(e);

  if (p == NULL) {
    fprintf(stderr, "Uncaught exception!");

    if (e->reason)
      fprintf(stderr, " %s", e->reason);
    else
      fprintf(stderr, " at 0x%p", (void *)e);

    if (file && line > 0)
      fprintf(stderr, " raised at [%s:%d]\n", file, line);

    fprintf(stderr, "Aborting...\n");
    fflush(stderr);

    abort();
  }

  p->file = file;
  p->line = line;
  p->exception = e;

  Except_stack = Except_stack->prev;

  longjmp(p->env, Except_raised);
}
