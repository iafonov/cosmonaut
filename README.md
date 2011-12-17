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

## Links

* Awesome networking programming guide: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
* `libevent` guide: http://www.wangafu.net/~nickm/libevent-book/
* C book: http://c.learncodethehardway.org/book/