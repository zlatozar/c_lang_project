## Basic Project Structure

```
├── AUTHORS
├── COPYING
├── ChangeLog
├── Makefile.am
├── README.md
├── configure.ac
├── bin
├── docs
├── dev_ops
├── deps
│   ├── bin
│   ├── obj
│   └── src
        ├─ proj1
        ...
├── config
│
└── src
    ├── lib1(bounded context)
    │   ├── bin
    │   ├── obj
    │   ├── src
    │   │   ├── file1.c
    │   │   ...
    │   │
    │   ├── include/
    │   │      │
    │   │      └── lib1
    │   │          │
    │   │          ├── public_interface.h
    │   │          ...
    │   ├── mocks
    │   │   ├── file1.c
    │   │   ...
    │   │
    │   ├── extern/
    │   │     │
    │   │     ...
    │   ├── README.md
    │   ├── Makefile.am
    │   └── tests
    │       ├── Makefile.am
    │       └── lib_test.c
    │
    ├── lib2
        ├ ...
   ... ...
    │
    ├── macros
    │   ├── README.md
    │   ├── Makefile.am
    │   ├── macros.h
    │   ...
    │
    └── core (domain logic)
        ├── Makefile.am
        ├── file1.c
        ├── main.c (entry point)
        └── fake_main.c (use mocked functions)
```

```
.gitmodules  projects that are in diferent repository
             builded and used as binaries

bin/     binaries
res/     data files for compilation but not source files
deps/    external libraries builded and used as binaries
log/     program logs
obj/     build object files
src/     source files
include/     header files
mock/        contains functions that emulate real behavior
doc/         documentation
dev_ops/     DevOps team
```

## Needed tools

- git
- gcc
- cmake
- autotools

## Dependencies

### Logging
[Log](https://github.com/rxi/log.c)

### Unit Testing
[Unity](https://github.com/ThrowTheSwitch/Unity)

### Neural Network
[Tiny Neural Network](https://github.com/glouw/tinn) <br/>
[Genann](https://github.com/codeplea/genann)

## Tools
1. [Doxygen](https://www.doxygen.nl/index.html) <br/>
2. [Valgrind](https://valgrind.org/)            <br/>
3. [AStyle](http://astyle.sourceforge.net/)     <br/>

## Books

### Basics
- _"From Hardware to Software: an introduction to computers"_ by Graham Lee
- _"Fundamentals of Operating Systems"_ by Eager

### C Language

#### Basics
- [Introduction to C](http://ix.cs.uoregon.edu/~norris/cis330books/ThinkingInC/Index.html)
- _"The C Programming Language"_ by Kernighan, Ritchie

### Standard Library Reference

[C Standard Library Headers](https://en.cppreference.com/w/c/header)

#### C Philosophy
- [Programming Paradigms lectures from Stanford](https://www.youtube.com/watch?v=Ps8jOj7diA0&list=PL5BD86273FEF4DB0B)
- [C exercism](https://exercism.io/tracks/c)
- _"Data Structures: An Advanced Approach Using C"_ by Jeffry Esakov <br/>
- _"C Interfaces and Implementations: Techniques for Creating Reusable Software"_ by David Hanson  <br/>
  [source code](https://github.com/zlatozar/cii)
- _"Functional C"_ by Hartel  <br/>
  [book and source](https://research.utwente.nl/en/publications/functional-c)


### Building Tools
- _"AUTOTOOLS. A Practitioner’s Guide to GNU Autoconf, Automake, and Libtool"_ by John Calcote

### Shell
- _"Linux & Unix Shell Programming"_ by David Tansley

### LEX&YACC
- _"Introduction to Compiler Construction with Unix"_ by Axel T. Schreiner

- _"Let's build a compiler"_ by by Jack Crenshaw (free book)  <br/>
  [source code](https://github.com/lotabout/Let-s-build-a-compiler)

### Domain Driven Design
- _"Domain Modeling Made Functional"_ by Scott Wlaschin   <br/>
   NOTE: Using functional way allow us to group nicely to code and also  <br/>
         introduce dependency injection using substitution of the dependent functions.
- _"Patterns, Principles, and Practices of Domain-Driven Design"_ by Scott Millett and Nick Tune

### OS
- _"The Logical Design of Operating Systems"_ by Alan C. Shaw
- _"Understanding UNIX/LINUX Programming: A Guide to Theory and Practice"_ by Bruce Molay

### OS Advanced Level
- _"Operating Systems: A Design Oriented Approach"_ by Crowley
- _"The Linux Programming Interface: A Linux and UNIX System Programming"_ by Michael Kerrisk
