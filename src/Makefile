PLATFORM := $(shell sh -c 'uname -s 2>/dev/null | tr 'a-z' 'A-Z'')
PREFIX?=/usr/local
CFLAGS=-g -Wall -D$(PLATFORM)

all: deps cosmonaut

cosmonaut: ../deps/dict/dict.o ../deps/strmap/strmap.o ../deps/concat/concat.o ../deps/url_parser/url.o ../deps/iniparser/dictionary.o ../deps/iniparser/iniparser.o ../deps/http_parser/http_parser.o routes_map.o platform.o file_util.o routing_engine.o action.o http_response.o base_request_handler.o configuration.o networking.o signals.o cli.o string_util.o http_request.o

# make dep
base_request_handler.o: base_request_handler.c base_request_handler.h \
  http_request.h ../deps/http_parser/http_parser.h \
  ../deps/url_parser/url.h http_response.h log.h
cli.o: cli.c cli.h configuration.h log.h
configuration.o: configuration.c log.h configuration.h cli.h networking.h \
  ../deps/iniparser/iniparser.h ../deps/iniparser/dictionary.h
cosmonaut.o: cosmonaut.c log.h signals.h networking.h \
  base_request_handler.h configuration.h
http_request.o: http_request.c http_request.h \
  ../deps/http_parser/http_parser.h ../deps/url_parser/url.h \
  string_util.h log.h configuration.h
http_response.o: http_response.c http_response.h
networking.o: networking.c networking.h configuration.h string_util.h \
  log.h
signals.o: signals.c log.h
string_util.o: string_util.c string_util.h

deps:
	cd ../deps && make

install: all
	install -d $(DESTDIR)/$(PREFIX)/bin/
	install cosmonaut $(DESTDIR)/$(PREFIX)/bin/

clean:
	cd ../deps && make clean
	rm -f *.o
	rm -f cosmonaut
	rm -rf *.dSYM

dep:
	$(CC) -MM *.c -I ../deps/iniparser/

