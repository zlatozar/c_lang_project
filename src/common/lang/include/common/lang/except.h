#ifndef COMMON_LANG_EXCEPT_H
#define COMMON_LANG_EXCEPT_H

#include <setjmp.h>

typedef struct Except_T {
  const char* message;
} Except_T;

typedef struct Except_Frame Except_Frame;

struct Except_Frame {
  Except_Frame *prev;
  jmp_buf env;
  const char* file;
  unsigned line;
  const Except_T* exception;
};

enum { Except_entered=0, Except_raised,
       Except_handled,   Except_finalized };

extern Except_Frame* Except_stack;

extern const Except_T Assert_Failed;

extern const Except_T Precondition_Failed;
extern const Except_T Postcondition_Failed;
extern const Except_T Invariant_Error;

void Except_raise(const Except_T* e, const char* file, unsigned line);

#define THROW(e) Except_raise(&(e), __FILE__, __LINE__)

#define RETHROW Except_raise(Except_frame.exception,             \
                             Except_frame.file, Except_frame.line)

#define RETURN switch (Except_stack = Except_stack->prev, 0) default: return

#define TRY do {                          \
  volatile int Except_flag;               \
  Except_Frame Except_frame;              \
  Except_frame.prev = Except_stack;       \
  Except_stack = &Except_frame;           \
  Except_flag = setjmp(Except_frame.env); \
  if (Except_flag == Except_entered) {

#define CATCH(e)                                                          \
    if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
  } else if (Except_frame.exception == &(e)) {                            \
    Except_flag = Except_handled;

#define ELSE                                                              \
    if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
  } else {                                                                \
    Except_flag = Except_handled;

#define FINALLY                                                           \
    if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
  } {                                                                     \
    if (Except_flag == Except_entered)                                    \
      Except_flag = Except_finalized;

#define END_TRY                                                           \
    if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } if (Except_flag == Except_raised) RETHROW;                          \
} while (0)

#endif  /* COMMON_LANG_EXCEPT_H */
