/* $Id */

// Convention - DIRECTORY_NAME_H
#ifndef MACROS_MATH_H
#define MACROS_MATH_H


#define PI             3.14159265

#define MIN(x, y)      ((x) < (y) ? (x) : (y))
#define MAX(x, y)      ((x) > (y) ? (x) : (y))

#define IS_ODD(num)    ((num) & 1)
#define IS_EVEN(num)   (!IS_ODD( (num) ))
#define IS_BETWEEN(num_to_test, num_low, num_high)                           \
  ((unsigned char)((num_to_test) >= (num_low) && (num_to_test) <= (num_high)))

#define COMPARE(x, y)  (((x) > (y)) - ((x) < (y)))

#endif /* MACROS_MATH_H */
