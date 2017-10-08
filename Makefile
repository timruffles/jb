

qj: qj.c
	gcc -Wall -Wextra -pedantic -Werror -Wshadow  -Wstrict-overflow -fno-strict-aliasing -std=c99 -O3 qj.c -o qj

PHONY: test
test: qj
	clear && prove -v t/conversion.t

PHONY: smoke-test
smoke-test: qj
	./qj oe  
