

qj: qj.c
	gcc -Wall -Werror -std=c99 -pedantic -O3 qj.c -o qj

PHONY: test
test: qj
	clear && prove -v t/conversion.t

PHONY: smoke-test
smoke-test: qj
	./qj oe  
