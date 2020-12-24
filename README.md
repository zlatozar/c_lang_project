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
│    └─ lib
│       ├── bin
│       ├── obj
│       ├── src
│       ...
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
    │   ├── bits.h
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
perf/        contain programs that check lib performance
test/        unit tests
examples/    usage examples
```

## Needed tools

- git
- gcc
- cmake
- autotools
- lex/yacc

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
