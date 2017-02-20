# berghaan - A reference implementation of 233-lisp

# Build

```sh
cd src/
aclocal
autoconf
automake --add-missing
autoreconf -ivf
./configure
make
```

After running commands above, an executable file `main` is created under the current directory, and it's runned as followed

```sh
./main < ../example/ex_1.233
```

# Examples

## Data Types

The implementation now supports the following types

* Boolean. Two valid values are `true` and `false`;
* Character. Such as 'a' and '汉'. The characters are read as UTF-8 bytes sequence;
* Function. Create by syntax `fun` or `defun`;
* Integer. Values like 1, 2, 3...;
* Map. Create by calling function `make-map`;
* NIL. The return type of some operators for only side-effect;
* String. A sequence of characters;
* Symbol. An identifier starts with a colon(:);
* Vector. A one dimension array, could contain values of any type
