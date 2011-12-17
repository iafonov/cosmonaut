default: dictionary.o iniparser.o

dictionary.o: dictionary.c dictionary.h
iniparser.o: iniparser.c iniparser.h dictionary.h

clean:
	rm -f *.o

dep:
	$(CC) -MM *.c -I ../deps/iniparser/