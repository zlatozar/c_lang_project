/* Validate project structure and build procedure */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>

#include "logger/log.h"
#include "lang/assert.h"
#include "data_structs/list.h"
#include "algorithms/array.h"

int
main(void)
{
  Assert(1);
  printf("%s\n", "Usage of printf....");

  log_debug("log_debug message");
  log_debug("log_debug message with %s", "parameter");
  log_info("log_info message");
  log_error("log_error message");
  log_error_if(1, "Condition is %d", 1);

  return EXIT_SUCCESS;
}
