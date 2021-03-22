/**
 * @file    interface.h
 * @brief   Kind of interface.
 */
#if !defined(POLYMORPHIC_INTERFACE_H)
#define POLYMORPHIC_INTERFACE_H

typedef struct interface_function_definitions {

  void* (*Interface_new)(void);

  int (*Interface_add)(void* interface_impl, int a, int b);

  void (*Interface_destroy)(void* interface_impl);

} interface_t;

typedef interface_t* Interface_T;

#endif  /* POLYMORPHIC_INTERFACE_H */
