include ../vars.mk
INC+=-I../inc

TESTS=$(patsubst %.c,bin/%,$(wildcard *.c))

bin: 
	mkdir -p $@

bin/%: %.c bin
	$(CC) $(CFLAGS) $(LIB) $(INC) $(LIBS) $< -o $@ $(LINK)

.PHONY: test clean 
test: $(TESTS)
	@./.test-runner.py

clean:
	rm -rf bin/


