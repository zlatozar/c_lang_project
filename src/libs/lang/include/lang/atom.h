#if !defined(LANG_ATOM_H)
#define LANG_ATOM_H

#include <stddef.h>

extern const char*  Atom_new   (const char* str, size_t len);
extern const char*  Atom_string(const char* str);

extern       size_t Atom_length(const char* str);

#endif  /* LANG_ATOM_H */
