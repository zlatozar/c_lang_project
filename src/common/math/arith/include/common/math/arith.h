/* $Id$ */

// Convention - DIRECTORY_NAME_H
#ifndef COMMON_MATH_ARITH_H
#define COMMON_MATH_ARITH_H

// _____________________________________________________________________________
//                                                                       Macros

#define PI             (3.14159265)

#define MIN(x, y)      ((x) < (y) ? (x) : (y))
#define MAX(x, y)      ((x) > (y) ? (x) : (y))

#define IS_ODD(num)    ((num) & 1)
#define IS_EVEN(num)   (!IS_ODD( (num) ))
#define IS_BETWEEN(num_to_test, num_low, num_high)                           \
  ((unsigned char)((num_to_test) >= (num_low) && (num_to_test) <= (num_high)))

#define COMPARE(x, y)  (((x) > (y)) - ((x) < (y)))

// _____________________________________________________________________________
//                                                                    Functions

extern int Arith_div(int x, int y);
extern int Arith_mod(int x, int y);

/// @brief Return the least integer not less than the real quotient of x/y. It is a
///        u.r.e. for y=0.
extern int Arith_ceiling(int x, int y);
extern int Arith_floor  (int x, int y);

#endif /* COMMON_MATH_ARITH_H */
