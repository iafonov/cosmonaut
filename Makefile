default: all

travis:

build: default

clean_test_app:
	cd features/test_app && make clean && rm -rf public/uploads/ && mkdir public/uploads/

build_test_app:
	cd features/test_app && make

travis_install:
	sudo make install

travis: init_submodules env clean build travis_install clean_test_app build_test_app
	export LD_LIBRARY_PATH="/usr/local/lib"
	./features/test_app/test_app
	bundle exec cucumber features/

test: clean build install clean_test_app build_test_app
	bundle install
	bundle exec cucumber features/

init_submodules:
	git submodule init
	git submodule update

env:
	export LD_LIBRARY_PATH="/usr/local/lib"

valgrind:
	valgrind --tool=memcheck --leak-check=full --show-reachable=yes ./src/cosmonaut

.DEFAULT:
	cd src && $(MAKE) $@
