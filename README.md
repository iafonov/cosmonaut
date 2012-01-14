# Cosmonaut

Fast web server & micro framework written in C. Just for fun.

The main idea behind cosmonaut - minimalism. It provides only minimal required functionality to process http requests and exposes as much details to application layer as possible. It could be used both as standalone application platform or as an underlying layer for a higher level framework powered by a higher level language.

The main purpose of using it as a standalone application is covering performance critical sections of application. For example you can use it to power/process your uploads or serve dynamic content which is sensitive to response time.

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

    void action_index(http_request *request, http_response *response) {
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

    typedef void (*action)(http_request *request, http_response *response);

## Request

Request structure features the basic data about request: parsed url, headers and params. It has two special fields `uid` and `configuration`. Server tags all requests with a unique identifier `uid` that could be used for logging. `configuration` is a pointer to structure with information about server configuration. You can retrieve data about server port, host, public root path and other various server runtime parameters. See configuration in-detail description further.

    struct http_request {
      url *url;
      headers_map *headers;
      params_map *params;
      char *uid;
      struct global_config *configuration;
    };

## URL

`url` structure represents parsed url parts.

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

  Headers are parsed and put into hash map. Here is some examples on how you can access header values:

    char *content_type = headers_map_get(request->headers, "Content-Type");

  If header is not present the accessor function will return `NULL`.

## Accessing parameters

### Parameters map

  Parameters both from request and from the url are parsed and put into `params` hash map.

    param_entry* param = params_map_get(request->params, "param_name");

### Parameter definition

  Parameter itself is represented as a structure which consists of parameter name, value and in case of multipart upload it would have uploaded file handler. You can use `is_file` flag to determine whether parameter value is a string or a file. File handler is closed so you'll have to reopen it if you'll want to modify file. Uploaded files by default are stored in `[uploads_root]/[request_uid]/[file_name]` path.

    typedef struct param_entry {
      char *name;
      char *val;

      bool is_file;
      FILE *file;
    } param_entry;

## Response

  Response is structure that you have to fill in order to send data to client. It consists of code, headers and response data. In most cases you shouldn't use this fields directly but rather it's more convenient to invoke on of several built-in rendering helpers or create your own to hide complexity and make your actions smaller and easier to understand.

    typedef struct http_response {
      int code;
      char *header_summary;
      char *file_path;
      char *content_type;
      int content_length;

      headers_map* headers;
      char* raw_response;
    } http_response;

## Rendering helpers

There are several pre-defined rendering helpers - they set correct corresponding headers and fill `http_response` object.

    render_file(http_response *response, const char *path);
    render_text(http_response *response, const char *text);
    render_json(http_response *response, const char *json);

## Routing

The routing system is very simple and straightforward but at the same time it is very powerful and you can use it as the first stage of routing and then do more tricky rerouting in your own wrapper.

### Simple action mounting

The most simple use-case is binding an action to the route. Whenever user will want to access this path you action would be executed.

    mount("/", action_index);
    mount("/upload_file", action_upload);

### Parameters capturing

Cosmonaut supports capturing named parameters. Everything between two slashes would be captured and put into the params map with corresponding name.

    mount("/photos/(:id)", action_show_photo);

    void action_show_photo(http_request *request, http_response *response) {
      int photo_id = params_map_get(request->params, "id")->val;
      ...
    }

### Advanced use cases

As mentioned before routing system could be used as a first stage of the more smart & application specific routing system. Here is use case which demonstrates mounting a RESTful application to a route.

    mount("/patients/(:id)/(:action)", patients_controller);

    void patients_controller(http_request *request, http_response *response) {
      char *action = params_map_get(request->params, "action")->val;
      int patient_id = params_map_get(request->params, "id")->val;

      ...

      process_data(patient, action, response);
    }

Examples of paths that would match this route:

    /patients/1/new
    /patients/1/show
    /patients/1/edit
    /patients/1/delete

### Handling 404 errors

If there are no actions that match requested route `[public_root]/404.html` would be rendered and appropriate http response code would be set.

## Configuration options

    [network]
    port = 31337;
    socket_queue_size = 80;
    server_name = 127.0.0.1;

    [app]
    public_root = ./public;
    uploads_root = ./public/uploads;

## Memory management

Cosmonaut follows the principle of a least surprise. Memory is freed only on the same level as it was allocated. So if you're passing chunks of data into any of built-in functions - your code is responsible for clean-up. The same principle is applied to built-in functions - if you're accessing data from params/headers you don't have to free it - the data from built-in data sources would be cleaned up automatically by framework functions which were responsible for allocating it.

# Under the hood

Cosmonaut uses very fast [http-parser](https://github.com/joyent/http-parser) to parse requests and very fast multipart data parser inspired by [node-formidable](https://github.com/felixge/node-formidable) module ported to C.

For now server has the simplest possible networking architecture it uses `accept` to get connections and forks a new process for each connection. Serving static content implemented using `sendfile` system call to send data directly to socket without buffering.

Routing engine is built on dynamically generated and complied during server boot time regular expressions.

## Conventions & project structure

Server is built from simple modules. Each module has two required function `module_init` which is responsible for allocating memory and setting initial values and `module_free` which is responsible for freeing memory. All module related function names start with `module_` prefix.

    typedef struct module;

    module* module_init();
    void module_free(module* m);

    void module_do_work(module *m, ...params);

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