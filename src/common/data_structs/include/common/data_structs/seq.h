#ifndef COMMON_DATA_STRUCTS_SEQ_H
#define COMMON_DATA_STRUCTS_SEQ_H


typedef struct Seq_T* Seq_T;

extern Seq_T  Seq_new(size_t hint);
extern Seq_T  Seq_seq(void* x, ...);
extern void   Seq_free(Seq_T* seq);

extern size_t Seq_length(Seq_T seq);

extern void*  Seq_get(Seq_T seq, unsigned i);
extern void*  Seq_put(Seq_T seq, unsigned i, void* x);

extern void*  Seq_addhi(Seq_T seq, void* x);
extern void*  Seq_addlo(Seq_T seq, void* x);

extern void*  Seq_remhi(Seq_T seq);
extern void*  Seq_remlo(Seq_T seq);

#endif  /* COMMON_DATA_STRUCTS_SEQ_H */
