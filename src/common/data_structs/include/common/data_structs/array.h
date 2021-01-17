#ifndef COMMON_DATA_STRUCTS_ARRAY_H
#define COMMON_DATA_STRUCTS_ARRAY_H


typedef struct _Array_T* Array_T;

extern Array_T Array_new(size_t length, size_t size);
extern void    Array_free(Array_T* array);

extern size_t  Array_length(Array_T array);
extern size_t  Array_size(Array_T array);

extern void*   Array_get(Array_T array, unsigned i);
extern void*   Array_put(Array_T array, unsigned i, void *elem);

extern void    Array_resize(Array_T array, size_t length);

extern Array_T Array_copy(Array_T array, size_t length);

#endif  /* COMMON_DATA_STRUCTS_ARRAY_H */
