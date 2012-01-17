default: all

travis:

build: default

clean_test_app:
	cd features/test_app && make clean && rm -rf public/uploads/ && mkdir public/uploads/

build_test_app:
	cd features/test_app && make

travis_install:
	sudo make install

travis: init_submodules clean build travis_install clean_test_app build_test_app
	export LD_LIBRARY_PATH="./src" && bundle exec cucumber features/

test: clean build install clean_test_app build_test_app
	bundle install
	bundle exec cucumber features/

init_submodules:
	git submodule init
	git submodule update

valgrind:
	valgrind --tool=memcheck --dsymutil=yes --leak-check=full --show-reachable=yes  ./features/test_app/test_app --config ./features/test_app/test_app.conf

.DEFAULT:
	cd src && $(MAKE) $@
