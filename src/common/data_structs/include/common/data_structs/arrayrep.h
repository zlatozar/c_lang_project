#ifndef COMMON_DATA_STRUCTS_ARRAYREP_H
#define COMMON_DATA_STRUCTS_ARRAYREP_H


struct Array_T {
  size_t length;
  size_t size;
  char* mem_array;
};

extern void ArrayRep_init(Array_T array, size_t length, size_t size, void *mem_loc);

#endif  /* COMMON_DATA_STRUCTS_ARRAYREP_H */
