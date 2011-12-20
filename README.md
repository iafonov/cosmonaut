# Learn C the über hard way

Fast multithreaded web server written in C. Just for fun.

## Lifecycle

### Start
* Parse CLI options
* Parse config file

### Bind to port
* Determine hostname

### Accept request
### Fork
* Validate request
* Determine request type
* Process request

### Catch TERM/INT signal & shutdown
* Close socket
* Do clean-up

# Build & run

Supported platforms: Ubuntu 10.04 (`build-essentials` is required), Mac OS X 10.6, 10.7 (XCode or gcc required).

1. Clone project
2. Cross your fingers
3. Run `make` it should build application with its dependencies
4. Run `make test` to run cucumber test suite
4. Run `./src/cosmonaut`

# HTTP

## Parsing

Super gotcha - be very careful with callbacks, if you mess with them parser behavior could be really weird.

* HTTP parser used in Node.js (based on nginx implementation): https://github.com/joyent/http-parser

## File upload
* HTTP file upload rfc: http://www.ietf.org/rfc/rfc1867.txt
* HTTP file upload rfc explained: http://www.vivtek.com/rfc1867.html

# Interesting implemenatation details

## Project structure

This is very very very important because the later you realize that your project structure is full of shit the harder it would be to do it in a right way. Use make, don't try to use rake for a C project. It doesn't work. (At least for me it didn't)

* http://c.learncodethehardway.org/book/learn-c-the-hard-waych27.html
* http://www.gnu.org/software/automake/manual/automake.html#Hello-World
* http://mij.oltrelinux.com/devel/autoconf-automake/

## Make

Hell on earth. Prepare drugs.

Gathering dependency info.

    dep:
      $(CC) -MM *.c -I ../deps/iniparser/

Forget about soft tabs, use only real tabs.

## Fork a process

Fork returns new process pid into new process and 0 to parent process thus everything inside `if` block would be executed in a new process. Don't forget to call `exit(0)` to prevent accidental fall-through.

    if (!fork()) {
      do_something();
      exit(0);
    }

* Awesome guide on IPC: http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html
* http://linux.die.net/man/2/fork

## Signals

`signal` is outdated - use `sigaction` instead. Set a function to catch & process signals.

* http://pubs.opengroup.org/onlinepubs/7908799/xsh/sigaction.html
* Pointers to functions overview: http://c.learncodethehardway.org/book/learn-c-the-hard-waych19.html#x24-9200019

## Signals & global variables

Use `sig_atomic_t` type to prevent firing signal handler callback in the middle of the read.

http://www.delorie.com/gnu/docs/glibc/libc_496.html

## Accept connection

`accept` - gets connection from queue and returns new socket file descriptor for it.

* http://linux.die.net/man/2/accept

## C99 structure in-place initialization

    static http_parser_settings settings = {
      .on_message_begin = message_begin_cb,
      .on_header_field = header_field_cb,
      .on_message_complete = message_complete_cb
    };

## Cool trick to eliminate useless variables

    int yes = 1;
    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, (&yes, sizeof(int)) == -1)

vs.

    setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, (int[]){1}, sizeof(int))

## Configuration parsing

ini-format file parser.

* https://github.com/ndevilla/iniparser

# Links

* Awesome networking programming guide: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
* `libevent` guide: http://www.wangafu.net/~nickm/libevent-book/
* C book: http://c.learncodethehardway.org/book/