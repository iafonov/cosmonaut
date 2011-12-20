PREFIX?=/usr/local
CFLAGS=-g -Wall

all: deps cosmonaut

cosmonaut: ../deps/url_parser/url.o ../deps/iniparser/dictionary.o ../deps/iniparser/iniparser.o ../deps/http_parser/http_parser.o base_request_handler.o configuration.o networking.o signals.o cli.o string_util.o http_request.o

# make dep
base_request_handler.o: base_request_handler.c base_request_handler.h \
  http_request.h ../deps/http_parser/http_parser.h log.h \
  ../deps/url_parser/url.h
cli.o: cli.c cli.h configuration.h log.h
cosmonaut.o: cosmonaut.c log.h signals.h networking.h \
  base_request_handler.h configuration.h
configuration.o: configuration.c log.h configuration.h cli.h \
  ../deps/iniparser/iniparser.h ../deps/iniparser/dictionary.h
http_request.o: http_request.c http_request.h \
  ../deps/http_parser/http_parser.h string_util.h log.h
networking.o: networking.c networking.h configuration.h log.h
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