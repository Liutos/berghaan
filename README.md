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
