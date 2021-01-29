#ifndef MACROS_STRING_H
#define MACROS_STRING_H


#define STR_ALLOC(str)          ( (char *)malloc(strlen(str) + 1) )
#define STR_CONCAT(str1, str2)  (str1 "" str2)
#define	STR_EQ(p, q)            (((*p)==(*q)) && (strcmp((p),(q)) == 0))

#define ISBLANK(c)              ((c) == '\t' || (c) == ' ')

#endif /* MACROS_STRING_H */
