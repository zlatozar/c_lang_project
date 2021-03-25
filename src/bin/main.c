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
#include "lang/memory.h"
#include "data_structs/list.h"
#include "algorithms/array.h"

int
main(void)
{
  Assert(1);
  printf("%s\n", "Usage of printf....");

  Log_debug("Log_debug message");
  Log_debug("Log_debug message with %s", "parameter");
  Log_info("Log_info message");
  Log_warn("Log_warn message");
  Log_error("Log_error message");
  Log_error_if(1, "Error condition is %d", 1);

  /* Manual indention for TRY/CATCH clauses is needed. */

  // *INDENT-OFF*
  TRY
    Log_error("Assert will fail...");
    Assert(0);

  ELSE
    Log_info("but exception is caught and application was shutdown gracefully!");

  FINALLY
    printf("Bye!\n");

  END_TRY;
  // *INDENT-ON*

  return EXIT_SUCCESS;
}
