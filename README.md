# To build

To use ncurses backend
```sh
cc src/main.c -DBACKEND_NCURSES -lncurses
```

To use using ansi escape code backend
```sh
cc src/main.c -DBACKEND_ANSI
```
