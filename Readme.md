## C project template

### Needed tools

- git
- gcc
- autotools (automake and autoconf)
- libtool
- make
- lex/yacc
- valgrind
- cppcheck
- astyle _(manual build)_
- fortune

### Build

Require `autotools` and `libtools`.
```text
$sudo dnf install autoconf automake libtool
```

```text
$ ./bootstrap.sh
$ make
$ make check cppcheck
$ make help
```
### Dependencies

#### Unit Testing

[greatest](https://github.com/silentbicycle/greatest)

#### Protothreads (co-routines)

[pt.h](http://dunkels.com/adam/pt/)

### Tools

1. [Doxygen](https://www.doxygen.nl/index.html) <br/>
2. [Valgrind](https://valgrind.org/)            <br/>
3. Cppcheck _(use linux package)_               <br/>
4. [AStyle](http://astyle.sourceforge.net/)
