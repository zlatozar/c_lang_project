#ifndef MACROS_DEBUG_H
#define MACROS_DEBUG_H


# define COLOR_RED     "\033[31m"
# define COLOR_GREEN   "\033[32m"
# define COLOR_RESET   "\033[0m"

#define DEBUG_PRINT(fmt, ...) do {                   \
    if (DEBUG)                                       \
      fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__,  \
              __LINE__, __func__, __VA_ARGS__);      \
  } while (0)

#define ENTER(fmt, args...) do {                    \
    if (DEBUG)                                      \
      fprintf(stderr,                               \
              COLOR_RED                             \
              "---------- " fmt " begin ----------\n"  \
              COLOR_RESET,                             \
              ## args);                                \
  } while (0)

#define EXIT(fmt, args...) do {                   \
    if (DEBUG)                                    \
      fprintf(stderr,                             \
              COLOR_GREEN                         \
              "---------- " fmt " end ----------\n"  \
              COLOR_RESET,                           \
              ## args);                              \
  } while (0)

#endif /* MACROS_DEBUG_H */
