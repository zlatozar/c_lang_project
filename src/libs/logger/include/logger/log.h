/**
 * @file     log.h
 * @brief    Logs with different levels
 */
#if !defined(LOGGER_LOG_H)
#define LOGGER_LOG_H

#include <stdio.h>     /* fprintf, stdout */
#include <string.h>    /* strrchr         */

#define NO_LOG         0x00
#define ERROR_LEVEL    0x01
#define INFO_LEVEL     0x02
#define DEBUG_LEVEL    0x03

#if !defined(DEBUG)
#  define LOG_COLOR    0
#  define LOG_LEVEL    ERROR_LEVEL
#else
#  define LOG_COLOR    1
#  define LOG_LEVEL    DEBUG_LEVEL
#endif

#if LOG_COLOR
#  define COLOR_BLACK  "\033[0;30m"
#  define COLOR_BLUE   "\033[1;34m"
#  define COLOR_RED    "\033[0;31m"
#  define COLOR_RESET  "\033[0m"
#else
#  define COLOR_BLACK
#  define COLOR_BLUE
#  define COLOR_RED
#  define COLOR_RESET
#endif

/* Forward declaration */
char* time_now(void);

#define __FILE    strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__

#ifdef LOG_COLOR
#  define LOG_FMT     "%s | %-16s | %-15s | %s:%d | "
#  define DEBUG_TAG   COLOR_BLACK "DEBUG" COLOR_RESET
#  define INFO_TAG    COLOR_BLUE "INFO" COLOR_RESET
#  define ERROR_TAG   COLOR_RED "ERROR" COLOR_RESET
#else
#  define LOG_FMT    "%s | %-5s | %-15s | %s:%d | "
#  define DEBUG_TAG   "DEBUG"
#  define INFO_TAG    "INFO"
#  define ERROR_TAG   "ERROR"
#endif  /* LOG_COLOR */

/* __________________________________________________________________________ */
/*                                                                Categories  */

#define _log_debug(msg, ...) do {                                   \
    if (LOG_LEVEL >= DEBUG_LEVEL)                                   \
      fprintf(stdout, LOG_FMT msg "%s",                             \
              time_now(), DEBUG_TAG,                                \
              __FILE, __func__, __LINE__, __VA_ARGS__); } while (0)

#define log_debug(...) do {                      \
    if (LOG_LEVEL >= DEBUG_LEVEL)                \
      _log_debug(__VA_ARGS__, "\n"); } while (0)

#define _log_info(msg, ...) do {                                    \
    if (LOG_LEVEL >= INFO_LEVEL)                                    \
      fprintf(stdout, LOG_FMT msg "%s",                             \
              time_now(), INFO_TAG,                                 \
              __FILE, __func__, __LINE__, __VA_ARGS__); } while (0)

#define log_info(...) do {                      \
    if (LOG_LEVEL >= INFO_LEVEL)                \
      _log_info(__VA_ARGS__, "\n"); } while (0)


#define _log_error(msg, ...) do {                                   \
    if (LOG_LEVEL >= ERROR_LEVEL)                                   \
      fprintf(stderr, LOG_FMT msg "%s",                             \
              time_now(), ERROR_TAG,                                \
              __FILE, __func__, __LINE__, __VA_ARGS__); } while (0)

#define log_error(...) do {                       \
    if (LOG_LEVEL >= ERROR_LEVEL)                 \
      _log_error(__VA_ARGS__, "\n"); } while (0)

#define log_error_if(condition, ...) do {          \
    if (condition)                                 \
      if (LOG_LEVEL >= ERROR_LEVEL)                \
        _log_error(__VA_ARGS__, "\n"); } while (0)

/* __________________________________________________________________________ */
/*                                                             Debug session  */

#define LOG_ENTER do {                                      \
    if (DEBUG)                                              \
      fprintf(stderr,                                       \
              COLOR_GREEN                                   \
              ">>>>>>>>>> ENTER: [%s:%d] %s() >>>>>>>>>>\n" \
              COLOR_RESET,                                  \
              __FILE__, __LINE__, __func__);                \
  } while (0);

#define LOG_EXIT do {                                       \
    if (DEBUG)                                              \
      fprintf(stderr,                                       \
              COLOR_RED                                     \
              "<<<<<<<<<< EXIT:  [%s:%d] %s() <<<<<<<<<<\n" \
              COLOR_RESET,                                  \
              __FILE__, __LINE__, __func__);                \
  } while (0);


#endif  /* LOGGER_LOG_H */
