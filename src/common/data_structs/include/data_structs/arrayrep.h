#ifndef COMMON_DATA_STRUCTS_ARRAYREP_H
#define COMMON_DATA_STRUCTS_ARRAYREP_H


struct array_rep {
  size_t length;
  size_t size;
  /* void* but we need pointer arithmetic */
  char* mem_alloc;
};

extern void ArrayRep_init(struct array_rep* p_arrayRep, size_t length,
                          size_t size, void* mem_alloc);

#endif  /* COMMON_DATA_STRUCTS_ARRAYREP_H */
