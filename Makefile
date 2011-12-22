default: all

test:
	bundle install
	bundle exec cucumber features/

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./src/cosmonaut

.DEFAULT:
	cd src && $(MAKE) $@
