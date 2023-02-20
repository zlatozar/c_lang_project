# Recommended C style and coding rules

This document describes C code style used by `Company Ltd.` in his projects.

## Table of Contents

  - [Naming convention rule](#naming-convention-rules)
  - [General rules](#general-rules)
  - [Comments](#comments)
  - [Functions](#functions)
  - [Variables](#variables)
  - [Structures, enumerations, typedefs](#structures-enumerations-typedefs)
  - [Compound statements](#compound-statements)
    - [Switch statement](#switch-statement)
  - [Macros and preprocessor directives](#macros-and-preprocessor-directives)
  - [Documentation](#documentation)
  - [Header/source files](#headersource-files)
  - [Artistic Style configuration](#artistic-style-configuration)

## Naming convention rules

- If you are using abbreviations like `usd`, `mph`, `kwh` put them as a suffix
  e.g. `sum_in_usd` and make sure you establish these ahead of time.
- Team have to agreed for common abbreviations and use them constantly.
- Avoid names with similar meanings.
- Suffix variables with `_str` if point to `(char*)` and should be treated as _string_.

## General rules

Here are listed most obvious and important general rules. Please check them
carefully before you continue with other chapters.

- Never have more than `100` characters per line
- Write to the most modern standard you can. `c11` is current
- Do not use **tabs**, use **spaces** instead
- Use `2` spaces per indent level
- Use `1` space between keyword and opening bracket
- Do not use space between _function name_ and opening bracket
```c
int32_t a = sum(4, 3);              /* OK */
int32_t a = sum (4, 3);             /* Wrong */
```

- Never use `__` or `_` prefix for variables/functions/macros/types. This is reserved for C language itself
- Use only lowercase characters for variables/functions/macros/types with optional underscore `_` char
- Opening curly bracket is always at the same line as keyword (`for`, `while`, `do`, `switch`, `if`, ...),
  only for methods are on the next line.
```c
size_t i;
for (i = 0; i < 5; ++i) {           /* OK */
}
for (i = 0; i < 5; ++i){            /* Wrong */
}
for (i = 0; i < 5; ++i)             /* Wrong */
{
}
void Arena_free(T arena)            /* Wrong */
{
}
```
- If you skip some of `for` construction leave a space
```c
for ( ; n-- > 0; p += s.len)        /* OK */
```

- Use single space before and after comparison and assignment operators
```c
int32_t a;
a = 3 + 4;              /* OK */
for (a = 0; a < 5; ++a) /* OK */
a=3+4;                  /* Wrong */
a = 3+4;                /* Wrong */
for (a=0;a<5;++a)       /* Wrong */
```

- Use single space after every comma
```c
func_name(5, 4);        /* OK */
func_name(4,3);         /* Wrong */
```

- Do not initialize `static` and `global` variables to `0` (or `NULL`), let compiler do it for you
```c
static int32_t a;       /* OK */
static int32_t b = 4;   /* OK */
static int32_t a = 0;   /* Wrong */

void
my_func(void)
{
  static int32_t* ptr;/* OK */
  static char abc = 0;/* Wrong */
}
```
- Minimize what you expose; declare top-level names `static` where you can.
- Declare local variables in order
    1. Custom structures, enumerations and they typedef
    2. Integer types, wider unsigned type first
    3. Single/Double floating point
```c
int
my_func(void)
{
  /* 1 */
  my_struct_t my;     /* First custom structures */
  my_struct_ptr_t* p; /* Pointers too */

  /* 2 */
  uint32_t a;
  int32_t b;
  uint16_t c;
  int16_t g;
  char h;
  /* ... */

  /* 3 */
  double d;
  float f;
}
```

- Always declare local variables at the beginning of the block, before first executable statement.
  Declaring variables when they're needed almost always leads to initialization
  (`int x = 1;`), rather than just declaration (`int x;`). Initializing a variable
  usually often means you can `const` it, too.

- Prefix constant names with `k`

- Prefix pointers with `p_`, double pointers with `pp_`

```c
/* OK */
int* p_counter;
List_T list;

List_T result = ylist;
List_T* p_result = &result;  /* Double pointer in practice */
```

- Declare counter variables in `for` loop
```c
/* OK */
for (size_t i = 0; i < 10; ++i)

/* OK, if you need counter variable later */
size_t i;
for (i = 0; i < 10; ++i) {
  if (...) {
    break;
  }
}
if (i == 10) {

}

/* Wrong */
size_t i;
for (i = 0; i < 10; ++i) ...
```

- Avoid variable assignment with function call in declaration, except for single variables
```c
void
a_name(void)
{
  /* Avoid function calls when declaring variables */
  int32_t a, b = sum(1, 2);

  /* Use this */
  int32_t a, b;
  b = sum(1, 2);

  /* This is ok */
  uint8_t a = 3;
  unit8_t b = 4;
}
```
**unsigned** should be a conscious choice that makes the developer think about potential risks,
used only there where you are **absolutely** sure that you can never go negative _(not even accidentally)_,
and that you need the additional value space.

- As a rough rule of thumb, use `unsigned int`s for **counting** things, and `signed int`s
  for **measuring** things (that goes up and down)
- _In most cases for function interfaces use signed and document that a variable is non-negative using assertions_
- Use `unsigned` types for bit values and bitwise operators
- Do not math with `unsigned` types.
- Avoid mixing signed and unsigned integers in the same expression. `-Wconversion` will take care for this
- Use integer **exact** types (eg. `int8_t`) when you need an integer object of an exact (fixed) length
- Use `size_t` as the type of an object, parameter or return type representing an **object size** in bytes.
- Use `int_least8_t` and so on to represent **small** integer values
- Use `double` rather than `float`, unless you have a specific reason otherwise
```c
/* OK */
uint8_t status;
status = 0;

/* OK */
#include <stdbool.h>
bool status = true;
```
Integer exact types defined in _most_ `<stdint.h>`:
```
    char                uint8_t   8     Unsigned       0 .. 255
    signed char         int8_t	  8     Signed      -128 .. 127
    unsigned short      uint16_t  16	Unsigned       0 .. 65,535
    short               int16_t	  16	Signed	 -32,768 .. 32,767

    unsigned int        uint32_t  32    Unsigned             0 .. 4,294,967,295
    int                 int32_t	  32	Signed	-2,147,483,648 .. 2,147,483,647

    unsigned long long	uint64_t  64	Unsigned             0 .. 18,446,744,073,709,551,615
    long long           int64_t   64    Signed  -9,223,372,036,854,775,808 .. 9,223,372,036,854,775,807
```
- Never compare against `true`, eg. `if (check_func() == 1)`, use `if (check_func()) { ... }`
- Always compare pointers against `NULL` value
```c
void* ptr;

/* ... */

/* OK, compare against NULL */
if (ptr == NULL || ptr != NULL) {

}

/* Wrong */
if (ptr || !ptr) {

}
```

- Always use *pre-increment (and decrement respectively)* instead of *post-increment (and decrement respectively)*
```c
int32_t a = 0;
...

a++;            /* Wrong */
++a;            /* OK */

for (size_t j = 0; j < 10; ++j) {}  /* OK */
```

- Always use `size_t` for _length_ or _size_ variables
- Always use `const` for pointer if function should **not** modify memory pointed to by `pointer`
- Always use `const` for function parameter or variable, if it should **not** be modified
- Immutability saves lives: use `const` everywhere you can
- `const` is a kind of documentation - user now knows what could be changed or not
```c
/* right-to-left, word is a constant pointer to a constant char */
char const* const word;

/* When d could be modified, data pointed to by d could not be modified */
void
my_func(const void* d)
{
}

/* When d and data pointed to by d both could not be modified */
void
my_func(const void* const d)
{
}

/* Not required, it is advised */
void
my_func(const size_t len)
{
}

/* When d should not be modified inside function, only data pointed to by d could be modified */
void
my_func(void* const d)
{
}
```

- When generic function may accept pointer of any type, always use `void *`
    - Client function must take care of proper casting in implementation
```c
/*
 * To send data, function should not modify memory pointed to by `data` variable
 * thus `const` keyword is important
 *
 * To send generic data (or to write them to file)
 * any type may be passed for data,
 * thus use `void *`
 */
/* OK example */
void
send_data(const void* data, size_t len)  /* OK */
{
    /* Do not cast `void *` or `const void *` */
    const uint8_t* d = data; /* Function handles proper type for internal usage */
}

void
send_data(const void* data, int len)  /* Wrong, not not use int */
{
}
```

- Always use brackets with `sizeof` operator
- Never use *Variable Length Array* (VLA). Use dynamic memory allocation instead
  with standard C `malloc` and `free` functions or if library/project provides
  custom memory allocation, use its implementation

```c
/* OK */
#include <stdlib.h>

void
my_func(size_t size)
{
  int32_t* arr;
  arr = malloc(sizeof(*arr) * n); /* OK, Allocate memory */
  arr = malloc(sizeof *arr * n);  /* Wrong, brackets for sizeof operator are missing */
  if (arr == NULL) {
    /* FAIL, no memory */
  }

  free(arr);  /* Free memory after usage */
}

/* Wrong */
void
my_func(size_t size)
{
  int32_t arr[size];  /* Wrong, do not use VLA */
}
```

- Non-Boolean values shall be converted to Boolean via use of relational
  operators (e.g., < or !=), not via casts.
- Never compare `boolean-treated` variables against zero or one. Use NOT (`!`) instead
```c
bool b_in_motion = (0 != speed_in_mph);  /* convert to boolean */

size_t length = 5;  /* Counter variable */
uint8_t is_ok = 0;  /* Boolean-treated variable */
if (length)         /* Wrong, length is not treated as boolean */
if (length > 0)     /* OK, length is treated as counter variable containing multi values, not only 0 or 1 */
if (length == 0)    /* OK, length is treated as counter variable containing multi values, not only 0 or 1 */

if (is_ok)          /* OK, variable is treated as boolean */
if (!is_ok)         /* OK, --||-- */
if (is_ok == 1)     /* Wrong, never compare boolean variable against 1! */
if (is_ok == 0)     /* Wrong, use ! for negative check */
```

- Never change state within an expression (e.g. with assignments or `++`)
```c
trie_add( *child, ++word );     /* Wrong */
trie_add( *child, word + 1 );   /* OK    */
```
- Always include check for `C++` with `extern` keyword in header file
- Every function in header file must include *doxygen-enabled* comment,
  corresponding C contains implementation could contains implementation details
- Use English names/text for functions, variables, comments
- Use *lowercase* characters for variables
- Use *underscore* if variable contains multiple names, eg. `force_redraw`. Do not use CamleCase `forceRedraw`
- Never cast function returning `void *`, eg. `uint8_t* ptr = (uint8_t *)func_returning_void_ptr();` as `void *` is safely promoted to any other pointer type
    - Use `uint8_t* ptr = func_returning_void_ptr();` instead
- Always use `<` and `>` for C Standard Library include files, eg. `#include <stdlib.h>`
- Always use `""` for **custom** libraries, eg. `#include "my_library.h"`
- When casting to pointer type, always align asterisk to type, eg. `uint8_t* t = (uint8_t*)var_width_diff_type`
- Always respect code style already used in project or library

## Comments

- Comments should provide something extra - not just paraphrase the code
- Comments starting with `//` are not allowed except for `Astyle` on/off formatting.<br/>
  Always use `/* comment */`, even for single-line comment
```c
// *INDENT-OFF*
// *INDENT-ON*

// This is comment (wrong)
/* This is comment (ok) */
```
- Comments should be complete sentences. The first word should be capitalized,
  unless it is an identifier that begins with a lower case letter
- For multi-line comments use `space+asterisk` for every line
```c
/*
 * This is multi-line comments,
 * written in 2 lines (ok)
 */

/**
 * Wrong, use double-asterisk only for doxygen documentation
 */

/*
* Single line comment without space before asterisk (wrong)
*/

/*
 * Single line comment in multi-line configuration (wrong)
 */

/* Single line comment (ok) */
```
- Comment sentences should end with dot.

## Functions

- Choose the right style for your API: change in place, or return changed structure:

```c
void
List_insert(List_T* p_List, Object_T data)
{
  node_t* p_node;
  List_allocate_node(&p_node, data);

  NEXT(p_node) = *p_List;
  *p_List = p_node;
}

/* Prefer side-effects. */
List_T list = List_new();
List_insert(&list, data1);
List_insert(&list, data2);
```

```c
List_T
List_insert(List_T list, Object_T data)
{
  node_t* p_node;
  List_allocate_node(&p_node, data);

  NEXT(p_node) = list;
  list = p_node;

  return list;
}

/* This is the style of the C starndard library. Allow nesting. */
List_T list = List_new();
list = List_insert(list, data1);
list = List_insert(List_insert(list, data2), data3);
```

- Be consistent in your variable names across functions
  Using different names for the same values in functions is suspicious, and
  forces your readers to reason about unimportant things.
- It is important to know that functions are not always inlined even if declared as such
- Every function which may have access from outside its module, must include
  function *prototype* (or *declaration*)
- Function name must start with module name in upper case followed by `_` and lowercase name
```c
/* OK */
Seq_T Seq_addlo(Seq_T seq, void* x)
void my_func(void);
void myfunc(void);

/* Wrong */
void MYFunc(void);
void myFunc();
```
- Local(private) functions should be prefixed with `__`
- Function return values or calculations that are obtained and then never change declare
  them `const`
```c
/* OK */
char* repeat_str(const char *str, size_t n)
{
  const size_t len = strlen(str);
  const size_t buf_size = 1 + (len * n);
  ...
}
```
- Never use `const` in a function prototype for a parameter passed by value. It
  has no meaning and is hence just 'noise'.
```c
/* Wrong */
int find(const int *data, size_t size, int value);
```
- When function returns pointer, align asterisk to return type
```c
/* OK */
const char* my_func(void);
my_struct_t* my_func(int32_t a, int32_t b);

/* Wrong */
const char *my_func(void);
my_struct_t * my_func(void);
```
- Align all function prototypes (with the same/similar functionality) for better readability
```c
/* OK, function names aligned */
void        set(int32_t a);
my_type_t   get(void);
my_ptr_t*   get_ptr(void);

/* Wrong */
void set(int32_t a);
const char * get(void);
```

- Function implementation should include return type and optional other keywords on top
```c
/* OK */
int32_t
foo(void)
{
  return 0;
}

/* OK */
static const char*
get_string(void) {
  return "Hello world!\r\n";
}
```

- When function returns pointer, asterisk character must be aligned to return type (`char*`)
```c
/* OK */
const char* foo(void)
{
  return "test";
}
```

- Extensively use `assert` to outline the function contract. See also project `assert.h`
- When defining function's parameters order is this: structure(changed by side-effects),
  then additional params
- If function has *output* parameters they should be at the end and suffixed with `__`,
  actual parameters(that will be passed) has suffix `_out`
```c
static Matrix_T
Matrix_get(Matrix_T m, int row, int col, void* p_value_) {
  ....
  *p_value_ = DATA(rowprev);
}
```
- Named functions parameters:
```c
struct _foo_args {
    int arg1;
    const char* arg2;
    void* arg3;
};

#define foo(...) _foo((struct _foo_args){__VA_ARGS__})
int _foo(struct _foo_args args) {
    /* Use arguments as args.arg1, args.arg2 and args.arg3 */
    /* Do some other things etc etc */
    return 0;
}

int main(void) {
    int result = foo(.arg2 = "Hello!");   // !!
    /* In the call above, arg1 is 0 and arg3 is NULL */
    return result;
}
```

## Variables

- Make variable name all lowercase with optional underscore `_` character
```c
/* OK */
int32_t a;
int32_t my_var;
int32_t myvar;

/* Wrong */
int32_t A;
int32_t myVar;
int32_t MYVar;
```

- Group local variables together by `type`
```c
void foo(void)
{
  int32_t a, b;   /* OK */

  char a;
  char b;         /* Wrong, there is char already */
}
```

- Do not declare variable after first executable statement
```c
void foo(void)
{
    int32_t a;
    a = bar();
    int32_t b;      /* Wrong, there is already executable statement */
}
```

- You may declare new variables inside next indent level
```c
int32_t a
a = foo();

if (a) {
    int32_t c, d;   /* OK, c and d are in if-statement scope */
    c = foo();
    int32_t e;      /* Wrong, there was already executable statement inside block */
}
```

- Declare pointer variables with asterisk aligned to type
```c
/* OK */
char* a;

/* Wrong */
char *a;
char * a;
```

## Structures, enumerations, typedefs

- Structure or enumeration name must be lowercase with optional underscore `_` character between words
- Structure or enumeration may contain `typedef` keyword
- All structure members must be lowercase
- All enumeration members must be uppercase
- Structure/enumeration must follow doxygen documentation syntax

When structure is declared, it may use one of `3` different options:

1. When structure is declared with *name only*, it *must not* contain `_t` suffix after its name.
```c
struct name {
  char* a;
  char b;
};
```
2. When structure is declared with *typedef only*, it *has to* contain `_t` suffix after its name.
```c
typedef struct {
  char* a;
  char b;
} name_t;
```
3. When structure is declared with *name and typedef*, it *must not* contain `_t` for basic name
   and it *has to* contain `_t` suffix after its name for typedef part.
```c
typedef struct name {
  char* a;
  char b;
  char c;
} name_t;
```
4. For opaque pointers use struct but staring with upper case and suffix `_T`.
```c
struct name {
  char* a;
  char b;
  char c;
};

typedef name* Name_T;
```

Examples of **bad declarations** and their suggested corrections
```c
/* a and b must be separated to 2 lines */
/* Name of structure with typedef must include _t suffix */
typedef struct {
  int32_t a, b;
} name;

/* Corrected version */
typedef struct {
  int32_t a;
  int32_t b;
} name_t;

/* Wrong name, it must not include _t suffix */
struct name_t {
  int32_t a;
  int32_t b;
};

/* Wrong parameters, must be all uppercase */
typedef enum {
  MY_ENUM_TESTA,
  my_enum_testb,
} my_tests_e;
```
- If appropriate all words should mean something in a given domain - DDD rules.
Example:
```c
#define RANK_NUM 13
#define DECK_NUM 52

typedef struct {
  int s_val;
  int r_val;
} card;

typedef struct name6 { char str[6]; } name6;
typedef struct rank { int v[RANK_NUM + 1]; } rank;
typedef struct deck { card v[DECK_NUM]; } deck;
...
void
print_card (deck d)
{
  name6 name_str;
  ...
}
```
- For every structure define macros for access and `typedefs` that hind the context of usage.
Example:
```c
struct node {
  Object_T datapointer;
  struct node* next;
};

/* Particular node from the list. */
typedef struct node node_t;
/* List as a whole (points to the first node). */
typedef node_t* List_T;

/* Kind of getters/setters. */
#define DATA(p_node) ((p_node)->datapointer)
#define NEXT(p_node) ((p_node)->next)

/* Define in any case. */
extern List_T List_new(void);
extern void List_free(List_T* p_list);

/* If needed only. */
#define EMPTY_LIST_NODE { .datapointer = NULL, .next = NULL }
```

- Enums `typedef` declarations ends with `_et`.
- Unions name `typedef` declarations ends `_ut`.

- When initializing structure on declaration, use `C99` initialization style
```c
/* OK */
name_t a = {
  .a = 4,
  .b = 5,
};

/* OK. Common pattern */
typedef struct {
  int id;
  char* name;
} employee_t;
#define INIT_EMPLOYEE(X) employee_t X = {.id = 0, .name ="none"}

/* Wrong */
a_t a = {1, 2};
```

- When new typedef is introduced for function handles, use `_FN` suffix to
  indicate that this is a functional interface
```c
/* Functions accept 2 parameters and returns uint8_t */
/* Name of typedef has `_FN` suffix */
typedef uint8_t (*my_func_typedef_FN)(uint8_t p1, const char* p2);
```

## Compound statements

- Every compound statement must include single indent; when nesting statements, include `1` indent size for each nest
```c
/* OK */
if (c) {
  do_a();
} else {
  do_b();
}

/* OK */
if (c)
  do_a();
else
  do_b();

/* Wrong */
if (c) do_a();
else do_b();
```

- In case of `if` or `if-else-if` statement, `else` must be in the same line as closing bracket of first statement
```c
/* OK */
if (a) {

} else if (b) {

} else {

}

/* Wrong */
if (a) {

}
else {

}

/* Wrong */
if (a) {

}
else
{

}
```

- In case of `do-while` statement, `while` part must be in the same line as closing bracket of `do` part
```c
/* OK */
do {
  int32_t a;
  a = do_a();
  do_b(a);
} while (check());

/* Wrong */
do
{
/* ... */
} while (check());

/* Wrong */
do {
/* ... */
}
while (check());
```

- Indentation is required for every opening bracket
```c
if (a) {
  do_a();
} else {
  do_b();
  if (c) {
    do_c();
  }
}
```

- Some time do compound statement without curly bracket
```c
/* OK */
if (a)
  do_b();
else
  do_c();

/* Wrong */
if (a) do_a(); else do_b();
```

- Empty `while`, `do-while` or `for` loops must include brackets
```c
/* OK */
while (is_register_bit_set()) {}

/* Wrong */
while (is_register_bit_set()) ;
while (is_register_bit_set()) { }
while (is_register_bit_set()) {
}
```

- If `while` (or `for`, `do-while`, etc) is empty (it can be the case in embedded programming), use empty single-line brackets
```c
/* Wait for bit to be set in embedded hardware unit
uint32_t* addr = HW_PERIPH_REGISTER_ADDR;

/* Wait bit 13 to be ready */
while (*addr & (1 << 13)) {}        /* OK, empty loop contains no spaces inside curly brackets */
while (*addr & (1 << 13)) { }       /* Wrong */
while (*addr & (1 << 13)) {         /* Wrong */

}
while (*addr & (1 << 13));          /* Wrong, curly brackets are missing. Can lead to compiler warnings or unintentional bugs */
```
- Always prefer using loops in this order: `for`, `while`, `do-while`
- Avoid incrementing variables inside loop block if possible, see examples

```c
/* Not recommended */
int32_t a = 0;
while (a < 10) {
    .
    ..
    ...
    ++a;
}

/* Better */
for (size_t a = 0; a < 10; ++a) {

}

/* Better, if inc may not happen in every cycle */
for (size_t a = 0; a < 10; ) {
  if (...) {
    ++a;
  }
}
```

### Switch statement

- Add *single indent* for every `case` statement
- Use additional *single indent* for `break` statement in each `case` or `default`
```c
/* OK, every case has single indent */
/* OK, every break has additional indent */
switch (check()) {
  case 0:
    do_a();
    break;
  case 1:
    do_b();
    break;
  default:
    break;
}

/* Wrong, case indent missing */
switch (check()) {
case 0:
  do_a();
  break;
case 1:
  do_b();
  break;
default:
  break;
}

/* Wrong */
switch (check()) {
  case 0:
        do_a();
    break;      /* Wrong, break must have indent as it is under case */
    case 1:
    do_b();     /* Wrong, indent under case is missing */
    break;
    default:
        break;
}
```

- Always include `default` statement
```c
/* OK */
switch (var) {
  case 0:
    do_job();
    break;
  default: break;
}

/* Wrong, default is missing */
switch (var) {
  case 0:
    do_job();
    break;
}
```

- If local variables are required, use curly brackets and put `break` statement inside.
    - Put opening curly bracket in the same line as `case` statement
```c
switch (a) {
  /* OK */
  case 0: {
    int32_t a, b;
    char c;
    a = 5;
    /* ... */
    break;
  }

  /* Wrong */
  case 1:
  {
    int32_t a;
    break;
  }

  /* Wrong, break shall be inside */
  case 2: {
    int32_t a;
  }
  break;
}
```

## Macros and preprocessor directives

- Always use macros instead of literal constants, specially for numbers
- All macros must be fully uppercase, with optional underscore `_` character, except if they
  are clearly marked as function which may be in the future replaced with regular function syntax
```c
/* OK */
#define MY_MACRO(x)         ((x) * (x))

/* Wrong */
#define square(x)           ((x) * (x))
```

- Always protect input parameters with parentheses
```c
/* OK */
#define MIN(x, y)           ((x) < (y) ? (x) : (y))

/* Wrong */
#define MIN(x, y)           x < y ? x : y
```

- Always protect final macro evaluation with parenthesis
```c
/* Wrong */
#define MIN(x, y)           (x) < (y) ? (x) : (y)
#define SUM(x, y)           (x) + (y)

/* Imagine result of this equation using wrong SUM implementation */
int32_t x = 5 * SUM(3, 4);  /* Expected result is 5 * 7 = 35 */
int32_t x = 5 * (3) + (4);  /* It is evaluated to this, final result = 19 which is not what we expect */

/* Correct implementation */
#define MIN(x, y)           ((x) < (y) ? (x) : (y))
#define SUM(x, y)           ((x) + (y))
```

- When macro uses multiple statements, protect it using `do-while (0)` statement

- Avoid using `#ifdef` or `#ifndef`. Use `defined()` or `!defined()` instead
```c
/* Wrong */
#ifdef XYZ
/* do something */
#endif /* XYZ */

/* OK */
#if defined(XYZ)
    /* Do if XYZ defined */
#else
    /* Do if XYZ not defined */
#endif
```

- Do indent sub statements inside `#if` statement
```c
/* OK */
#if defined(MAXALIGN)
    char pad[MAXALIGN];
#else
    int i;
    long l;
    long double ld;
#endif

/* OK */
#if !defined(DEBUG)
#  define LOG_COLOR    0
#else
#  define LOG_COLOR    1
#endif

```

## Documentation

Documented code allows doxygen to parse and general html/pdf/latex output, thus it is very important to do it properly.

- Use doxygen-enabled documentation style for `variables`, `functions` and `structures/enumerations`
- Do use `@`
```c
/**
 * @brief Holds pointer to first entry in linked list
 *        Beginning of this text is 5 tabs (20 spaces) from beginning of line
 */
static type_t* list;
```

- Every structure/enumeration member must include documentation if not obvious
```c
/**
 * @brief This is point struct
 * @note  This structure is used to calculate all point
 *        related stuff
 */
typedef struct {
  int32_t x;      /*!< Point X coordinate */
  int32_t y;      /*!< Point Y coordinate */
  int32_t size;   /*!< Point size.
                       Since comment is very big,
                       you may go to next line */
} point_t;

/**
 * @brief Point color enumeration
 */
typedef enum {
  COLOR_RED,     /*!< Red color. This comment has 12x4
                      spaces offset from beginning of line */
  COLOR_GREEN,   /*!< Green color */
  COLOR_BLUE,    /*!< Blue color */
} point_color_e;
```

- Documentation for functions must be written in header files, implementation
  notes as comments in function implementation.
- Function must include `brief` and all parameters documentation
- Every parameter must be noted if it is `in` or `out` for *input* and *output* respectively
- Function must include `return` parameter if it returns something. This does not apply for `void` functions
- Function can include other doxygen keywords, such as `note` or `warning`
- Use colon `:` between parameter name and its description
```c
/**
 * Sum `2` numbers
 *
 * @param[in]  a: First number
 * @param[in]  b: Second number
 * @return     Sum of input values
 */
int32_t sum(int32_t a, int32_t b)
{
  return a + b;
}

/**
 * Sum `2` numbers and write it to pointer
 *
 * @note This function does not return value, it stores it to pointer instead
 * @param[in]   a: First number
 * @param[in]   b: Second number
 * @param[out]  result: Output variable used to save result
 */
void void_sum(int32_t a, int32_t b, int32_t* result_)
{
  *result = a + b;
}
```

- If function returns member of enumeration, use `ref` keyword to specify which one
```c
/**
 * My enumeration
 */
typedef enum {
  MY_ERR,     /*!< Error value */
  MY_OK       /*!< OK value */
} my_enum_t;

/**
 * Check some value
 * @return  \ref MY_OK on success, member of \ref my_enum_t otherwise
 */
my_enum_t check_value(void)
{
  return MY_OK;
}
```

- Use notation (\`NULL\` => `NULL`) for constants or numbers
```c
/**
 * Get data from input array
 *
 * @param[in]  in: Input data
 * @return     Pointer to output data on success, `NULL` otherwise
 */
const void* get_data(const void* in)
{
  return in;
}
```

- Documentation for macros must include `hideinitializer` doxygen command
```c
/**
 * Get minimal value between `x` and `y`
 *
 * @param[in]       x: First value
 * @param[in]       y: Second value
 * @return          Minimal value between `x` and `y`
 * \hideinitializer
 */
#define MIN(x, y)       ((x) < (y) ? (x) : (y))
```

## Header/source files

- Leave single empty line at the end of file
- Every file must include doxygen annotation for `file` and `brief` description followed by empty line (when using doxygen)
```c
/**
 * @file   template.h
 * @brief  Template include file
 */
           /* Here is empty line */
```
- Comment `#includes` to say what symbols you use from them
Finding where things come from is always one of the main challenges when learning a codebase.
```c
#include <limits.h>  /* INT_MAX      */
#include <stddef.h>  /* NULL, size_t */

```
- Every file (*header* or *source*) must include license (opening comment includes single asterisk as this must be ignored by doxygen)
- Use the same license as already used by project/library
```c
/**
 * @file   template.h
 * @brief  Template include file
 */

/*
 * Copyright (c) year FirstName LASTNAME
 *
 *
 * This file is part of 'library_name'.
 *
 * Author: FirstName LastName <optional_email@example.com>
 */
```

- Header file must include guard `#ifndef`
- Always comment `#endifs` of large conditional sections
- Header file must include `C++` check
- Include external header files outside `C++` check
- Header file must include only every other header file in order to compile
  correctly, but not more (.c should include the rest if required)

A header file must include the header files that directly define each of the
types directly used in or that directly declare each of the functions used in
the header file in question, but must not include anything else.

- Header file must only expose module **public** variables/types/functions
- Use `extern` for global module variables in header file, define them in source file later
```c
/* file.h ... */
#ifndef ...

extern int32_t my_variable; /* This is global variable declaration in header */

#endif

/* file.c ... */
int32_t my_variable;        /* Actually defined in source */
```
- Never include `.c` files in another `.c` file
- `.c` file should first include corresponding `.h` file, then others, unless otherwise explicitly necessary
  Always `#include A.h` first to avoid hiding anything it is missing that gets
  included by other `.h` files. Then, if `A`'s implementation code uses `X`, explicitly
  `#include X.h` in `A.c`, so that `A.c` is not dependent on `X.h` accidentally being `#included` somewhere else.
- Do not include module private declarations in header file
- Include definitions of **data structures** and **enumerations** that are shared amongst multiple source files
- The order of declarations in header file and there definitions in .c file should be the same

- Header file example (no license for sake of an example)
```c
/**
 * @file    template.h
 * @brief   Template header file
 */

/* License comes here */
#if !defined(TEMPLATE_HDR_H)
#define TEMPLATE_HDR_H

/* Include headers */

#ifdef __cplusplus
extern "C" {
#endif        /* __cplusplus */

/* File content here */

#ifdef __cplusplus
}
#endif        /* __cplusplus */

#endif  /* TEMPLATE_HDR_H */
```
- Header file should contain structure definitions and methods prototypes all `typedefs` that are used
  in the implementation are not obliged let client defines its owns if needed.
```c
struct node {
  Object_T datapointer;
  struct node* next;
};

/* Not required it's a matter of taste */
typedef struct node node_t;

```

### Header file best practices

- Each module with its .h and .c file should correspond to a clear piece of functionality
  Don’t force together into a module things that will be used or maintained
  separately, and don’t separate things that will always be used and maintained
  together.
- Always use “include guards” in a header file. Guard name should be PATH\_MODULE\_H
  Note: _Do not start the guard symbol with an underscore!_ Leading underscore
  names are reserved for internal C.
- All of the declarations needed to use a module must appear in its header file,
  and this file is always used to access the module.
- Keep a module’s internal declarations out of the header file
- Every header file `A.h` should `#include` every other header file that `A.h`
  requires to compile correctly, but no more. The content of a header file
  should compile correctly by itself. Check your headers by compiling (by
  itself) a `test.c` that contains nothing more than `#include A.h`.

## Artistic style configuration

[AStyle](http://astyle.sourceforge.net/) is a great piece of software that can
help with formatting the code based on input configuration.

This repository contains `tools-setup/astylerc` file which can be used to
setup `AStyle` software.

## Useful libraries

- [libtab](https://github.com/zorgnax/libtap)
- [mps](https://github.com/orangeduck/mpc)
- [coroutine](https://github.com/cloudwu/coroutine)
- [Thread pool](https://github.com/Pithikos/C-Thread-Pool)
- [AVL implementation](https://github.com/etherealvisage/avl)
