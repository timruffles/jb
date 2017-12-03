

qj: qj.c
	gcc -Wall -Wextra -pedantic -Werror -Wshadow  -Wstrict-overflow -fno-strict-aliasing -std=c99 -O3 qj.c -o qj

tiny: tiny.c
	gcc -Wall -Wextra -pedantic -Werror -Wshadow  -Wstrict-overflow -fno-strict-aliasing -std=c99 -O3 tiny.c -o tiny


PHONY: test
test: qj
	clear && prove -v t/conversion.t

PHONY: test-reference
test-reference:
	clear && TEST_CMD="node jb.js" prove -v t/conversion.t

PHONY: smoke-test
smoke-test: qj
	./qj oe  
