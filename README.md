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

## Special Operators

This term `special operator` is borrowed from Common Lisp, and here they're

### set for assignment

The operator `set` is used to assign a value to a variable

```lisp
(set a 1)
```

Here `a` is the name of a variable and `1` is the value assigned to it

### fun for function creation

The operator `fun` is similar to the famous `lambda`, it's used for creating a function object immediately

```lisp
(fun (x) (+ x 1))
```

### defun for defining global functions

`defun` is similar to `fun`, except that it's used to *define* a *global* function, and it would not create a function object in place. It's almost equivalent to create a function by means of `fun` and assign this function to a variable by means of `set`

```lisp
(defun bar (x) (+ x 1))
```

### if for conditional

`if` is easy to use. The first argument to it is the test case, and if the expression in test case evaluated to a `true`, the second argument, named consequence, would be evaluated. Otherwise, the third argument, named alternate, would be evaluated

```lisp
(if true 1 2)
```

The expression above return value `1`
