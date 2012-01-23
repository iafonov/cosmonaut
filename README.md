# Cosmonaut

[![Build Status](https://secure.travis-ci.org/iafonov/cosmonaut.png)](http://travis-ci.org/iafonov/cosmonaut)

Fast web server & micro framework implemented in C. Just for fun.

The main idea behind cosmonaut - minimalism. It provides only minimal required functionality to process http requests and exposes as much details to application layer as possible. It could be used both as standalone application platform or as an underlying layer for a higher level framework powered by a higher level language. It doesn't have any external dependencies.

The main purpose of using it as a standalone application is covering performance critical sections of application. For example you can use it to power/process your uploads or serve dynamic content which is sensitive to response time.

Also you can get some ideas behind Cosmonaut from [slides](http://speakerdeck.com/u/iafonov/p/web-servers-and-how-i-created-my-own-one) from my talk given on [CnC Donetsk](http://cnc.dn.ua/) meeting.

## Building & installation

Supported platforms: Ubuntu (`build-essentials` is required), Mac OS X 10.6, 10.7 (XCode or gcc required). Both static & dynamic libraries are provided.

1. Clone project
2. Cross your fingers
3. Run `make` to build application with its dependencies
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

More advanced example is available [here](https://github.com/iafonov/uploader_app).

## Action

Action is a simple function that accepts `http_request` and `http_response` structs as parameters and fills the last one with the data according to the request data.

    typedef void (*action)(http_request *request, http_response *response);

## Request

Request structure holds the basic data about request: parsed url, headers and params. It has two special fields `uid` and `configuration`. Server tags all requests with a unique identifier `uid` that could be used for logging. `configuration` is a pointer to structure with information about server configuration. You can retrieve data about server port, host, public root path and other various server runtime parameters. See configuration in-detail description further. `route` is struct which represents selected route for this request. Route is selected by routing engine immediately after first line of request is parsed. `data` is a pointer to your own piece of data. It could be used to persist and share state between filters, hooks and actions. Canonical use case is keeping connection to database between filters and action.

    struct http_request {
      url *url;
      headers_map *headers;
      params_map *params;
      configuration *configuration;
      route *route;
      void *data;
      char *uid;
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

  Response is structure that you have to fill in order to send data to client. It consists of code, headers and response data. In most cases you shouldn't use this fields directly but rather it's more convenient to invoke one of several built-in rendering helpers or create your own to hide complexity and make your actions smaller and easier to understand.

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

The routing system is very simple and straightforward but at the same time it's very powerful and you can use it as the first stage of routing and then do some tricky and application specific rerouting in your own wrapper.

### Simple action mounting

The most simple use-case is binding an action to the route. Whenever user will access this path your action would be executed.

    mount("/", action_index);
    mount("/upload_file", action_upload);

### Parameters capturing

Cosmonaut supports capturing named parameters. Everything between two slashes would be captured and put into the params map with the corresponding name.

    mount("/photos/(:id)", action_show_photo);

    void action_show_photo(http_request *request, http_response *response) {
      int photo_id = params_map_get(request->params, "id")->val;
      ...
    }

### Advanced use cases

As mentioned before routing system could be used as the first stage of the more smart & application specific routing system. Here is use case which demonstrates mounting a RESTful application to the route.

    mount("/patients/(:id)/(:action)", patients_controller);

    void patients_controller(http_request *request, http_response *response) {
      char *action = params_map_get(request->params, "action")->val;
      int patient_id = params_map_get(request->params, "id")->val;

      ...

      process_data(patient, action, response);
    }

Examples of paths that would match this route:

    /patients/1/new    // params: {id => 1, action => "new"}
    /patients/1/show   // params: {id => 1, action => "show"}
    /patients/1/edit   // params: {id => 1, action => "edit"}
    /patients/1/delete // params: {id => 1, action => "delete"}

### Handling 404 errors

If there are no actions that match the requested route `[public_root]/404.html` would be rendered and appropriate http response code would be set.

### Filters

Filters are simple callbacks that are executed at various points of request lifecycle.

    typedef void (*filter)(http_request* request);

You can set filters by attaching callbacks to routes in your `configure` function.

    route* rt = mount("/files/new/(:upload_id)", action_upload);
    rt->before_filter = check_upload_size;
    rt->after_filter = clear_db_connections;

### Hooks

Hooks are specific callbacks that are called during request parsing process. The main difference between hooks and filters is in their definition & interface - hooks are more specific and filters are more general. For now there is only on hook: `progress_hook` - it is called every time after new chunk of data is processed. It passes already parsed amount and content length. It could be used for tracking upload progress and must be setup in route's `before_filter`.

    route* rt = mount("/files/new/(:upload_id)", action_upload);
    rt->before_filter = set_progress_hook;

    ...

    void set_progress_hook(http_request* request) {
      request->progress_hook = capture_progress;
    }

    void capture_progress(int content_length, int parsed) {
      info("Progress: %d -> %d", parsed, content_length);
    }

## Configuration options

    [network]
    port = 31337;
    socket_queue_size = 80;
    server_name = 127.0.0.1;

    [app]
    public_root = ./public;
    uploads_root = ./public/uploads;

## Memory management

Cosmonaut follows the principle of a least surprise. Memory is freed only at the same level as it was allocated. So if you're passing chunks of data into any of built-in functions - your code is responsible for clean-up. The same principle is applied backwards to built-in functions - if you're accessing data from params/headers you don't have to free it, the data from built-in  sources would be cleaned up automatically by framework functions which were responsible for allocating it.

# Under the hood

Cosmonaut uses very fast [http-parser](https://github.com/joyent/http-parser) to parse requests and very fast [multipart data parser](https://github.com/iafonov/multipart-parser-c) inspired by [node-formidable](https://github.com/felixge/node-formidable) module ported to C.

For now server has the simplest possible networking architecture - it uses `accept` to get connections and forks a new process for each connection. Serving static content implemented using `sendfile` system call to send data directly to socket without buffering.

Routing engine is built using regular expressions. Regexes are dynamically generated and complied during server boot time so there is very little overhead during parsing requests.

## Conventions & project structure

Server is built from simple modules. Each module has two required function `module_init` which is responsible for allocating memory and setting initial values and `module_free` which is responsible for freeing memory. All module related function names start with `module_` prefix.

    typedef struct module;

    module* module_init();
    void module_free(module* m);

    void module_do_work(module* m, ...);

## Testing

Application features acceptance test suite built with [cucumber](https://github.com/cucumber/cucumbe) & [rest-client](https://github.com/archiloque/rest-client). Run `make && make test` to run acceptance test suite. (Requires ruby & bundler)

# License & copyright

All code except `src/dict.h`, `src/dict.c` and all code in `deps` folder is released under the [MIT License](http://www.opensource.org/licenses/mit-license.php).

© 2012 [Igor Afonov](http://iafonov.github.com)
