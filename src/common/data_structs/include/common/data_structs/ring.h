#ifndef COMMON_DATA_STRUCTS_RING_H
#define COMMON_DATA_STRUCTS_RING_H


typedef struct _Ring_T* Ring_T;

extern Ring_T Ring_new(void);
extern Ring_T Ring_ring(void* x, ...);
extern void   Ring_free(Ring_T* ring);

extern int    Ring_length(Ring_T ring);

extern void*  Ring_get(Ring_T ring, int i);
extern void*  Ring_put(Ring_T ring, int i, void* x);

extern void*  Ring_add(Ring_T ring, int pos, void* x);
extern void*  Ring_addhi(Ring_T ring, void* x);
extern void*  Ring_addlo(Ring_T ring, void* x);

extern void*  Ring_remove(Ring_T ring, int i);
extern void*  Ring_remhi(Ring_T ring);
extern void*  Ring_remlo(Ring_T ring);

extern void   Ring_rotate(Ring_T ring, int n);

#endif  /* COMMON_DATA_STRUCTS_RING_H */
