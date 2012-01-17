default: all

travis:

build: default

clean_test_app:
	cd features/test_app && make clean && rm -rf public/uploads/ && mkdir public/uploads/

build_test_app:
	cd features/test_app && make

travis: init_submodules clean build clean_test_app build_test_app
	export LD_LIBRARY_PATH="./src" && bundle exec cucumber features/

test: clean build install clean_test_app build_test_app
	export LD_LIBRARY_PATH="./src" && bundle exec cucumber features/

init_submodules:
	git submodule init
	git submodule update

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./src/cosmonaut

.DEFAULT:
	cd src && $(MAKE) $@
