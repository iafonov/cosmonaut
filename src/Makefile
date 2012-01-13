OBJ = ../deps/url_parser/url.o \
      ../deps/iniparser/dictionary.o \
      ../deps/iniparser/iniparser.o \
      ../deps/http_parser/http_parser.o \
      cosmonaut.o \
      params_map.o \
      mpart_body_processor.o \
      attrs_map.o \
      multipart_parser.o \
      str_map.o \
      log.o \
      headers_map.o \
      routes_map.o \
      platform.o \
      file_util.o \
      routing_engine.o \
      action.o \
      http_response.o \
      base_request_handler.o \
      configuration.o \
      networking.o \
      signals.o \
      cli.o \
      string_util.o \
      http_request.o \
			url.o \
			dict.o

OPTIMIZATION?=-O0
PLATFORM := $(shell sh -c 'uname -s 2>/dev/null | tr 'a-z' 'A-Z'')

ifeq ($(PLATFORM),DARWIN)
  CFLAGS?=$(OPTIMIZATION) -fPIC -Wall $(ARCH) $(PROF) -D$(PLATFORM)
  LDFLAGS?=-L. -Wl,-rpath,.
  OBJARCH?=-arch i386 -arch x86_64
  DYLIBNAME?=libcosmonaut.dylib
  DYLIB_MAKE_CMD?=libtool -dynamic -o ${DYLIBNAME} -lm ${DEBUG} - ${OBJ}
  STLIBNAME?=libcosmonaut.a
  STLIB_MAKE_CMD?=libtool -static -o ${STLIBNAME} - ${OBJ}
else
  CFLAGS?=$(OPTIMIZATION) -fPIC -Wall -g $(ARCH) $(PROF) -D$(PLATFORM)
  LDFLAGS?=-L. -Wl,-rpath,.
  DYLIBNAME?=libcosmonaut.so
  DYLIB_MAKE_CMD?=gcc -shared -Wl,-soname,${DYLIBNAME} -o ${DYLIBNAME} ${OBJ}
  STLIBNAME?=libcosmonaut.a
  STLIB_MAKE_CMD?=ar rcs ${STLIBNAME} ${OBJ}
endif

CCOPT= $(CFLAGS) $(CCLINK)
DEBUG?= -g -ggdb

PREFIX?= /usr/local
INSTALL_INC= $(PREFIX)/include/cosmonaut
INSTALL_LIB= $(PREFIX)/lib
INSTALL= cp -a

all: deps cosmonaut ${DYLIBNAME} ${STLIBNAME}

cosmonaut: ${OBJ}

${DYLIBNAME}: ${OBJ}
	${DYLIB_MAKE_CMD}

${STLIBNAME}: ${OBJ}
	${STLIB_MAKE_CMD}

dep:
	$(CC) -MM *.c

deps:
	cd ../deps && make

install: ${DYLIBNAME} ${STLIBNAME}
	mkdir -p $(INSTALL_INC) $(INSTALL_LIB)
	$(INSTALL) *.h $(INSTALL_INC)
	$(INSTALL) ${DYLIBNAME} ${STLIBNAME} $(INSTALL_LIB)

clean:
	cd ../deps && make clean
	rm -f *.o
	rm -f cosmonaut-example
	rm -f libcosmonaut.*
	rm -rf *.dSYM

