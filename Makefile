default: all

test:
	bundle install
	bundle exec cucumber features/

.DEFAULT:
	cd src && $(MAKE) $@