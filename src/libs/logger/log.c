#include "logger/log.h"

#include <time.h>

char*
time_now(void)
{
  time_t rawtime;
  time(&rawtime);

  struct tm* timeinfo = localtime(&rawtime);

  static char buffer[64];
  strftime(buffer, 64, "%Y-%m-%d %H:%M:%S", timeinfo);

  return buffer;
}
