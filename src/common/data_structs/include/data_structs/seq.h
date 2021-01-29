#ifndef COMMON_DATA_STRUCTS_SEQ_H
#define COMMON_DATA_STRUCTS_SEQ_H


typedef struct seq* Seq_T;

extern Seq_T  Seq_new(size_t hint);
extern Seq_T  Seq_seq(void* x, ...);
extern void   Seq_free(Seq_T* self);

extern size_t Seq_length(Seq_T self);

extern void*  Seq_get(Seq_T self, unsigned i);
extern void*  Seq_put(Seq_T self, unsigned i, void* x);

extern void*  Seq_addhi(Seq_T self, void* x);
extern void*  Seq_addlo(Seq_T self, void* x);

extern void*  Seq_remhi(Seq_T self);
extern void*  Seq_remlo(Seq_T self);

#endif  /* COMMON_DATA_STRUCTS_SEQ_H */
