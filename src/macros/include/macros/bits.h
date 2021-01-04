#ifndef MACROS_BITS_H
#define MACROS_BITS_H


#define BIT(x)           (1 << (x))
#define SETBIT(x, p)     ((x) | (1 << (p)))
#define CLEARBIT(x, p)   ((x) & (~(1 << (p))))
#define GETBIT(x, p)     (((x) >> (p)) & 1)
#define TOGGLEBIT(x, p)  ((x) ^ (1 << (p)))
#define LSB(x)           ((x) ^ ((x) - 1) & (x))

#endif /* MACROS_BITS_H */
