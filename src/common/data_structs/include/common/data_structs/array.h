#ifndef COMMON_DATA_STRUCTS_ARRAY_H
#define COMMON_DATA_STRUCTS_ARRAY_H


typedef struct array_rep* Array_T;

extern Array_T Array_new(size_t length, size_t size);
extern void    Array_free(Array_T* self);

extern size_t  Array_length(Array_T self);
extern size_t  Array_size(Array_T self);

extern void*   Array_get(Array_T self, unsigned i);
extern void*   Array_put(Array_T self, unsigned i, void* elem);

extern void    Array_resize(Array_T self, size_t length);

extern Array_T Array_copy(Array_T self, size_t length);

#endif  /* COMMON_DATA_STRUCTS_ARRAY_H */
