// Convention - DIRECTORY_NAME_H
#ifndef COMMON_MATH_ARITH_H
#define COMMON_MATH_ARITH_H

// _____________________________________________________________________________
//                                                                       Macros

#define PI             (3.14159265)

#define IS_ODD(num)    ((num) & 1)
#define IS_EVEN(num)   (!IS_ODD( (num) ))

// _____________________________________________________________________________
//                                                                    Functions

extern int Arith_max(int x, int y);
extern int Arith_min(int x, int y);

extern int Arith_div(int x, int y);
extern int Arith_mod(int x, int y);

/// Return the least integer not less than the real quotient of x/y.
/// It is a run time error for y=0.
extern int Arith_ceiling(int x, int y);
extern int Arith_floor  (int x, int y);

#endif /* COMMON_MATH_ARITH_H */
