/* Validate project structure and build procedure */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <errno.h>
#include "lang/assert.h"

int
main(void)
{
  Assert(1);
  printf("%s\n", "Just testing project template...");
  return EXIT_SUCCESS;
}
