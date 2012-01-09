default: all

build: default

clean_test_app:
	cd features/test_app && make clean && rm -rf public/uploads/ && mkdir public/uploads/

build_test_app:
	cd features/test_app && make

test: clean build clean_test_app build_test_app install
	bundle install
	bundle exec cucumber features/

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./src/cosmonaut

.DEFAULT:
	cd src && $(MAKE) $@
