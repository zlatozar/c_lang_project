/**
 * @file    try.h
 * @brief   Kind of polymorphism.
 */
#if !defined(DATA_STRUCTS_TRY_H)
#define DATA_STRUCTS_TRY_H

typedef struct itry {
  void* (*ITry_new)(void);
  int (*ITry_add)(void* itry, int a, int b);
  void (*ITry_destroy)(void* itry);
} itry_t;

typedef itry_t* ITry_T;

#endif  /* DATA_STRUCTS_TRY_H */
