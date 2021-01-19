#ifndef COMMON_DATA_STRUCTS_RING_H
#define COMMON_DATA_STRUCTS_RING_H


typedef struct ring* Ring_T;

extern Ring_T Ring_new(void);
extern Ring_T Ring_ring(void* x, ...);
extern void   Ring_free(Ring_T* self);

extern int    Ring_length(Ring_T self);

extern void*  Ring_get(Ring_T self, int i);
extern void*  Ring_put(Ring_T self, int i, void* x);

extern void*  Ring_add(Ring_T self, int pos, void* x);
extern void*  Ring_addhi(Ring_T self, void* x);
extern void*  Ring_addlo(Ring_T self, void* x);

extern void*  Ring_remove(Ring_T self, int i);
extern void*  Ring_remhi(Ring_T self);
extern void*  Ring_remlo(Ring_T self);

extern void   Ring_rotate(Ring_T self, int n);

#endif  /* COMMON_DATA_STRUCTS_RING_H */
