#include "lang/except.h"

#include <stdlib.h>      /* abort                        */
#include <stdio.h>       /* fprintf, stderr, fflush      */
#include <execinfo.h>    /* backtrace, backtrace_symbols */
#include "lang/assert.h"

#define STACK_BUFF 64

static void
except_backtrace(void)
{
  void* buffer[STACK_BUFF];

  /* Last two are not useful. */
  int size = backtrace(buffer, STACK_BUFF) - 2;
  fprintf(stderr, "backtrace() stack:\n");

  char** names_str = backtrace_symbols(buffer, size);

  if (names_str == NULL)
  { return; }

  /*
   * NOTE: In the stack trace number of next after the problem
   *       is returned.
   */
  for (int i = 0; i < size; i++) {
    char* line = names_str[i];
    fprintf(stderr, "[%d] %s\n", i, line);
  }

  fprintf(stderr,
          "To see stacktrace lines use: addr2line -pfCe <executable> <addresses>\n\n");

  free(names_str);
}

/* Initialize exception stack before usage. */
Except_Frame* Except_stack = NULL;

void
Except_throw(const Except_T* e, const char* file, int line)
{
  Require(e);

  /* Check if this exception is handled. */
  Except_Frame* p = Except_stack;

  if (p == NULL) {
    fprintf(stderr, "\n\nUncaught exception!");

    if (e->message)
    { fprintf(stderr, "  Message: '%s'", e->message); }
    else
    { fprintf(stderr, "  at 0x%p", (void*)e); }

    if (file && line > 0)
    { fprintf(stderr, " Raised at [%s: %d]\n", file, line); }

#if defined(__linux__) && defined(DEBUG)
    except_backtrace();
#else
    fprintf(stderr, "Aborting...\n");
#endif
    fflush(stderr);

    abort();
  }

  p->file = file;
  p->line = line;
  p->exception = e;

  /* Wind exception stack. */
  Except_stack = Except_stack->prev;

  longjmp(p->env, Except_raised);
}
