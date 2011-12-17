PREFIX?=/usr/local
CFLAGS=-g -Wall
LDFLAGS=-lapr-1 -pthread -laprutil-1

all: cosmonaut

cosmonaut: base_request_handler.o global_config.o networking.o signals.o

install: all
	install -d $(DESTDIR)/$(PREFIX)/bin/ 
	install devpkg $(DESTDIR)/$(PREFIX)/bin/ 

clean:
	rm -f *.o 
	rm -f devpkg 
	rm -rf *.dSYM