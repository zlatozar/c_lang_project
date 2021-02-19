#if !defined(LANG_EXCEPT_H)
#define LANG_EXCEPT_H

#include <setjmp.h>

typedef struct Except_T {
  const char* message;
} Except_T;

typedef struct Except_Frame Except_Frame;

struct Except_Frame {
  Except_Frame* prev;
  jmp_buf env;
  const char* file;
  int line;
  const Except_T* exception;
};

enum { Except_entered = 0, Except_raised,
       Except_handled, Except_finalized
     };

/* GLOBAL */
extern Except_Frame* Except_stack;
/* GLOBAL */
extern const Except_T Assert_Failed;
extern const Except_T Precondition_Failed;
extern const Except_T Postcondition_Failed;
extern const Except_T Invariant_Error;

/**
 * @brief    Raises exception `e` at source coordinate file and line.
 *
 * It is a run-time error if exception is `NULL`.
 * Uncaught exceptions cause program termination.
 */
void Except_raise(const Except_T* e, const char* file, int line);

#define THROW(e) Except_raise(&(e), __FILE__, __LINE__)

#define RETHROW Except_raise(Except_frame.exception,             \
                             Except_frame.file, Except_frame.line)

/**
 * @brief     A return statement used within TRY statements.
 *
 * It is a run-time error to use a C `return` statement
 * in TRY statements.
 *
 * @note Expression also can be returned: `RETURN exp`
 */
#define RETURN switch (Except_stack = Except_stack->prev, 0) default: return

#define TRY do {                            \
    volatile int Except_flag;               \
    Except_Frame Except_frame;              \
    Except_frame.prev = Except_stack;       \
    Except_stack = &Except_frame;           \
    Except_flag = setjmp(Except_frame.env); \
    if (Except_flag == Except_entered) {

#define CATCH(e)                                                            \
      if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } else if (Except_frame.exception == &(e)) {                            \
      Except_flag = Except_handled;

#define ELSE                                                                \
      if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } else {                                                                \
      Except_flag = Except_handled;

#define FINALLY                                                             \
      if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } {                                                                     \
    if (Except_flag == Except_entered)                                      \
      Except_flag = Except_finalized;

#define END_TRY                                                             \
      if (Except_flag == Except_entered) Except_stack = Except_stack->prev; \
    } if (Except_flag == Except_raised) RETHROW;                            \
  } while (0)

#endif  /* LANG_EXCEPT_H */
