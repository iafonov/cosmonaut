# Cosmonaut

Fast web server & micro framework written in C. Just for fun.

The main idea behind cosmonaut - minimalism. It provides only minimal required functionality to process http requests and exposes as much details to application layer as possible. It could be used both as standalone application platform or as an underlying layer for a higher level framework powered by a higher level language.

The main purpose of using it as a standalone application is covering performance critical sections of application. For example you can use it to power/process your uploads or serve static content.

## Under the hood

Cosmonaut uses very fast [http-parser](https://github.com/joyent/http-parser) to parse requests and very fast multipart data parser inspired by [node-formidable](https://github.com/felixge/node-formidable) module but implemented in C.

For now server has the simplest possible networking architecture it uses `accept` to get connections and forks a new process for each connection. Serving static content implemented using `sendfile` system call to send data directly to socket without buffering.

## Building & installation

Supported platforms: Ubuntu 10.04 (`build-essentials` is required), Mac OS X 10.6, 10.7 (XCode or gcc required). Both static & dynamic libraries are provided.

1. Clone project
2. Cross your fingers
3. Run `make` it should build application with its dependencies
4. Run `make test` to run test suite
5. Run `make install` to install both static and dynamic libraries and headers

# Lifecycle

Server lifecycle is very simple and straightforward:

1. Parse request
2. Match action with request path
3. Call action which will fill response
4. Send response back to client

## Hello world

    #include "cosmonaut.h"

    void action_index(http_request* request, http_response *response) {
      render_text(response, "Hello world");
    }

    void configure() {
      mount("/", action_index);
    }

    int main(int argc, char *argv[]) {
      return cosmonaut_start(argc, argv, configure);
    }

## Action

Action is a simple function that accepts `http_request` and `http_response` structs as parameters and fills the last one with the data according to request data.

    typedef void (*action)(http_request* request, http_response* response);

## Request

Request structure features the basic data about request: parsed url, headers and params. It has two special fields `uid` and `configuration`. Server tags all requests with a unique identifier `uid`. `configuration` is a pointer to structure with information about server configuration. You can retrieve data about server port, host, public root path and other various server runtime parameters. 

    struct http_request {
      url* url;
      headers_map* headers;
      params_map* params;
      char* uid;
      struct global_config* configuration;
    };

## URL

This structure represents parsed url parts.

    typedef struct url {
      char *scheme;
      char *host;
      char *port;
      char *path;
      char *query;
      char *fragment;
      char *username;
      char *password;
    } url;

## Accessing headers

## Accessing parameters

## Response

    typedef struct http_response {
      int code;
      char* header_summary;
      char* file_path;
      char* content_type;
      int content_length;

      headers_map* headers;
      char* raw_response;
    } http_response;

## Rendering helpers

There are several pre-defined rendering helpers - they set correct corresponding headers and put data to `http_response` object (like `content-type`, `content_length` etc.)

    render_file(http_response *response, const char *path);
    render_text(http_response *response, const char *text);
    render_json(http_response *response, const char *json);

## Routing

## Configuration options

# Links

#### Parsers

* HTTP parser used in Node.js (based on nginx implementation): https://github.com/joyent/http-parser
* Multipart parser: https://github.com/felixge/node-formidable
* INI key-value file parser: https://github.com/ndevilla/iniparser

#### File upload
* HTTP Multipart form data rfc: http://www.ietf.org/rfc/rfc2388.txt
* HTTP file upload rfc: http://www.ietf.org/rfc/rfc1867.txt
* HTTP file upload rfc explained: http://www.vivtek.com/rfc1867.html

#### Sendfile

* Mac OS X reference: http://developer.apple.com/library/mac/#documentation/Darwin/Reference/ManPages/man2/sendfile.2.html
* Linux: http://kernel.org/doc/man-pages/online/pages/man2/sendfile.2.html

#### Project structure

* http://c.learncodethehardway.org/book/learn-c-the-hard-waych27.html
* http://www.gnu.org/software/automake/manual/automake.html#Hello-World
* http://mij.oltrelinux.com/devel/autoconf-automake/

#### IPC

* Awesome guide on IPC: http://beej.us/guide/bgipc/output/html/singlepage/bgipc.html
* http://linux.die.net/man/2/fork

#### Signals

* http://pubs.opengroup.org/onlinepubs/7908799/xsh/sigaction.html
* Pointers to functions overview: http://c.learncodethehardway.org/book/learn-c-the-hard-waych19.html#x24-9200019
* http://www.delorie.com/gnu/docs/glibc/libc_496.html

#### Other

* Awesome networking programming guide: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
* `libevent` guide: http://www.wangafu.net/~nickm/libevent-book/
* C book: http://c.learncodethehardway.org/book/