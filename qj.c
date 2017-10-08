# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>

# define EOS '\0'

static char SEPARATOR = ' ';

static int (*nextChar)();
static void pickInput();
static int nextCharArgc();
static void runOperator();

// a pointer to argv, which is an array of pointers to chars
static char **argv;
static int argc;

typedef char OperatorLiteral[3];


int main(int argc_, char* argv_[]) {
  argc = argc_;
  argv = (char**) argv_;
  pickInput();
  printf("address of argv: %p\n", (void*)argv_);
  // first 
  printf("value of argv[0]: %c\n", argv_[0][0]);

  // try to read an operator
  OperatorLiteral operator;
  char c;

  while(1) {
    // consume next two letters
    operator[0] = 0;
    operator[1] = 0;
    operator[2] = 0;
    printf("hi operator %s\n", operator);
    int consumed = 0;
    for(consumed = 0; consumed < 3 && ((c = nextChar()) != EOF); consumed++) {
      operator[consumed] = c;
    }

    if(operator[0] != SEPARATOR && operator[1] != SEPARATOR && (operator[2] == SEPARATOR || operator[2] == EOF || operator[2] == 0)) {
      runOperator(operator);
    } else {
      printf("not op\n");
    }
  }

  return 0;
}

bool isOp(char a, char b, OperatorLiteral op) {
  return a == op[0] && b == op[1];
}

void output(char* string) {
  puts(string);
}

void objectEmpty() {
  output("{}");
}

void runOperator(OperatorLiteral operator) {
  if(isOp('o','e',operator)) {
    objectEmpty();
    return;
  }
  fprintf(stderr, "failed - unknown operator '%s'", operator);
  exit(1);
}

// decide where we're reading our source-code from
void pickInput() {
  if(argc == 1) {
    nextChar = getchar;
  } else {
    nextChar = nextCharArgc;
  }
}

int nextCharArgc() {
  static int positionArgumentsConsumed = 1;
  static int currentCharacterIndex = 0;

  printf("nextCharArgc %i %i %i\n", positionArgumentsConsumed, currentCharacterIndex, argc);

  // argc 0 is program, so once we've consumed argc - 1 words we're done
  if(positionArgumentsConsumed >= argc) {
    return EOF;
  }

  // pick the next character from our current argument
  char *argument = argv[positionArgumentsConsumed];
  char nextCharacter = argument[currentCharacterIndex];
  if(nextCharacter == EOS) {
    positionArgumentsConsumed += 1;
    currentCharacterIndex = 0;
    return nextCharArgc();
  }

  currentCharacterIndex += 1;
  return nextCharacter;
}

