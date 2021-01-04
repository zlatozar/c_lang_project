#ifndef COMMON_DATA_STRUCTS_ATOM_H
#define COMMON_DATA_STRUCTS_ATOM_H


extern const char  *Atom_new(const char *str, size_t len);

extern       size_t Atom_length(const char *str);

extern const char  *Atom_int(long n);
extern const char  *Atom_string(const char *str);

#endif
