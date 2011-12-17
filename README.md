# Learn C the über hard way

## Description

Fast multithreaded web server written in C. Just for fun.

### Lifecycle

#### Bind to port
#### Accept request & fork
#### Do clean-up

## Interesting things

### Fork a process

Fork returns new process pid into new process and 0 to parent process thus everything inside `if` block would be executed in a new process. Don't forget to call `exit(0)` to prevent accidental fall-trough.

    if (!fork()) {
      do_something();
      exit(0);
    }

* http://linux.die.net/man/2/fork

### Signals

`signal` is outdated - use `sigaction` instead. Set a function to catch & process signals.

* http://pubs.opengroup.org/onlinepubs/7908799/xsh/sigaction.html
* Pointers to functions overview: http://c.learncodethehardway.org/book/learn-c-the-hard-waych19.html#x24-9200019

### Signals & global variables

Use `sig_atomic_t` type to prevent firing signal handler callback in the middle of the read.

http://www.delorie.com/gnu/docs/glibc/libc_496.html

### Accept connection

`accept` - gets connection from queue and returns new socket file descriptor for it.

* http://linux.die.net/man/2/accept

## Links

* Awesome networking programming guide: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
* `libevent` guide: http://www.wangafu.net/~nickm/libevent-book/
* C book: http://c.learncodethehardway.org/book/