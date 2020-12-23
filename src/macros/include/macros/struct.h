/* $Id */

// Convention - DIRECTORY_NAME_H
#ifndef MACROS_STRUCT_H
#define MACROS_STRUCT_H


/* Obtain the offset of a field in a structure */
#define GET_FIELD_OFFSET(struct_name, field_name)     \
  ((short)(long)(&((struct_name *)NULL) -> field_name))

/* Obtain the struct element at the specified offset */
#define GET_FIELD_PTR(p_struct, offset)   \
  ((void *)(((char *)p_struct) + (offset)))

/*
 * Allocates a structure given the structure name and returns a pointer to
 * that allocated structure.
 */
#define ALLOC_STRUCT(struct_name)              \
  ((struct_name *)malloc( sizeof(struct_name) ))

/* Initializes the given structure to zeroes */
#define ZERO_INIT_STRUCT(p_struct) (memset( p_struct, '\0', sizeof( *(p_struct) )))

#endif /* MACROS_STRUCT_H */
