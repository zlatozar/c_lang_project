#if !defined(LANG_DEBUG_H)
#define LANG_DEBUG_H

#include <stdio.h>  /* fprintf, stderr */

#ifdef DEBUG
#define DEBUG 1
#else
#define DEBUG 0
#endif

# define COLOR_RED     "\033[31m"
# define COLOR_GREEN   "\033[32m"
# define COLOR_RESET   "\033[0m"

#define debug_print(fmt, ...) do {           \
    if (DEBUG)                               \
      fprintf(stderr, "[%s:%d] %s(): "       \
              COLOR_RED fmt COLOR_RESET,     \
              __FILE__, __LINE__, __func__,  \
              __VA_ARGS__); } while (0)

#define ENTER do {                                          \
    if (DEBUG)                                              \
      fprintf(stderr,                                       \
              COLOR_GREEN                                   \
              ">>>>>>>>>> ENTER: [%s:%d] %s() >>>>>>>>>>\n" \
              COLOR_RESET,                                  \
              __FILE__, __LINE__, __func__);                \
  } while (0);

#define EXIT do {                                            \
    if (DEBUG)                                               \
      fprintf(stderr,                                        \
              COLOR_RED                                      \
              "<<<<<<<<<< EXIT:  [%s:%d] %s() <<<<<<<<<<\n"  \
              COLOR_RESET,                                   \
              __FILE__, __LINE__, __func__);                 \
  } while (0);

#endif  /* LANG_DEBUG_H */
