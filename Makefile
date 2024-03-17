include vars.mk


TESTS+=$(wildcard tests/*.c*)
HDRS+=$(wildcard inc/*.h)


#    ___  _             _     ___              
#   |   \(_)_ _ ___   _| |_  |   \ ___ _ __ ___
#   | |) | | '_(_-<  |_   _| | |) / -_) '_ (_-<
#   |___/|_|_| /__/    |_|   |___/\___| .__/__/
#                                     |_|      

gitman_sources:
	pip install gitman
	gitman install
	# install the pre-commit hook
	cp .pre-commit .git/hooks

#    ___ _             _        
#   | _ \ |_  ___ _ _ (_)___ ___
#   |  _/ ' \/ _ \ ' \| / -_|_-<
#   |_| |_||_\___/_||_|_\___/__/
#                               
.PHONEY: docs deps clean test format

test:
	make -C tests test

format:
	clang-format -i $(SRCS)
	clang-format -i $(HDRS)
	clang-format -i $(TESTS)

docs:
	mkdir -p $@
	doxygen

deps:
	edit .gitman.yaml

deps-update:
	gitman update

clean:
	rm -rf obj lib docs
	make -C tests clean
