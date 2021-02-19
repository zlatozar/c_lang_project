#include "lang/except.h"

#include <stdlib.h>      /* abort                   */
#include <stdio.h>       /* fprintf, stderr, fflush */
#include "lang/assert.h"

/* Initialize exception stack before usage */
Except_Frame* Except_stack = NULL;

void
Except_raise(const Except_T* e, const char* file, int line)
{
  Require(e);

  Except_Frame* p = Except_stack;

  if (p == NULL) {
    fprintf(stderr, "Uncaught exception!");

    if (e->message)
    { fprintf(stderr, " %s", e->message); }
    else
    { fprintf(stderr, " at 0x%p", (void*)e); }

    if (file && line > 0)
    { fprintf(stderr, " raised at [%s: %d]\n", file, line); }

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
