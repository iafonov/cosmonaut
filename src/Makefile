PREFIX?=/usr/local
CFLAGS=-g -Wall

all: cosmonaut

cosmonaut: base_request_handler.o global_config.o networking.o signals.o cli.o ../deps/iniparser/dictionary.o ../deps/iniparser/iniparser.o

# make dep
base_request_handler.o: base_request_handler.c base_request_handler.h \
  log.h
cli.o: cli.c cli.h global_config.h
cosmonaut.o: cosmonaut.c log.h signals.h networking.h \
  base_request_handler.h global_config.h cli.h
global_config.o: global_config.c log.h global_config.h \
  ../deps/iniparser/iniparser.h ../deps/iniparser/dictionary.h
networking.o: networking.c networking.h global_config.h log.h
signals.o: signals.c log.h

install: all
	install -d $(DESTDIR)/$(PREFIX)/bin/
	install cosmonaut $(DESTDIR)/$(PREFIX)/bin/

clean:
	rm -f *.o
	rm -f cosmonaut
	rm -rf *.dSYM

dep:
	$(CC) -MM *.c -I ../deps/iniparser/