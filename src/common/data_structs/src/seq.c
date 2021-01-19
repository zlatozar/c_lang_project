#include <stdarg.h>  /* va_list, va_start */
#include <string.h>  /* memcpy            */

#include "macros/lang.h"
#include "common/lang/assert.h"
#include "common/lang/mem.h"
#include "common/data_structs/array.h"
#include "common/data_structs/arrayrep.h"

#include "common/data_structs/seq.h"

struct seq {
  struct array_rep storage;
  size_t length;
  int head;
};

LOCAL void expand(Seq_T seq)
{
  int n = seq->storage.length;
  Array_resize(&seq->storage, 2*n);

  if (seq->head > 0) {
    void** old = &((void **)seq->storage.mem_alloc)[seq->head];
    memcpy(old + n, old, (n - seq->head) * sizeof(void *));
    seq->head += n;
  }
}

Seq_T Seq_new(size_t hint)
{
  Seq_T seq;
  NEW_0(seq);

  if (hint == 0)
    hint = 16;

  ArrayRep_init(&seq->storage, hint, sizeof(void *), ALLOC(hint * sizeof(void *)));

  return seq;
}

Seq_T Seq_seq(void* x, ...)
{
  va_list ap;

  Seq_T seq = Seq_new(0);

  va_start(ap, x);

  for ( ; x; x = va_arg(ap, void *))
    Seq_addhi(seq, x);

  va_end(ap);

  return seq;
}

void Seq_free(Seq_T* seq)
{
  Assert(seq && *seq);
  Assert((void *)*seq == (void *) & (*seq)->storage);

  Array_free((Array_T *)seq);
}

size_t Seq_length(Seq_T seq)
{
  Assert(seq);
  return seq->length;
}

void* Seq_get(Seq_T seq, unsigned i)
{
  Assert(seq);
  Assert(i < seq->length);

  return ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length];
}

void* Seq_put(Seq_T seq, unsigned i, void* x)
{
  void* prev;

  Assert(seq);
  Assert(i < seq->length);

  prev = ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length];
  ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length] = x;

  return prev;
}

void* Seq_addhi(Seq_T seq, void* x)
{
  int i;

  Assert(seq);

  if (seq->length == seq->storage.length)
    expand(seq);

  i = seq->length++;

  return ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length] = x;
}

void* Seq_addlo(Seq_T seq, void* x)
{
  int i = 0;

  Assert(seq);

  if (seq->length == seq->storage.length)
    expand(seq);

  if (--seq->head < 0)
    seq->head = seq->storage.length - 1;

  seq->length++;

  return ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length] = x;
}

void* Seq_remhi(Seq_T seq)
{
  int i;

  Assert(seq);
  Assert(seq->length > 0);

  i = --seq->length;
  return ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length];
}

void* Seq_remlo(Seq_T seq)
{
  int i = 0;
  void* x;

  Assert(seq);
  Assert(seq->length > 0);

  x = ((void **)seq->storage.mem_alloc)[(seq->head + i) % seq->storage.length];
  seq->head = (seq->head + 1) % seq->storage.length;

  --seq->length;

  return x;
}
