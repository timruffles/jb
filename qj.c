# include <stdio.h>

static char SEPARATOR = ' ';

static int (*nextChar)();
static void pickInput();
static int nextCharArgc();

// a pointer to argv, which is an array of pointers to chars
static char **argv;
static int argc;

int main(int argc_, char* argv_[]) {
  argc = argc_;
  argv = argv_;
  pickInput(argc_, argv_);
  printf("address of argv: %p\n", (void*)argv_);
  // first 
  printf("value of argv[0]: %c\n", argv_[0][0]);

  // try to read an operator
  char operator[3];
  char c;
  operator[2] = 0;
  printf("hi operator %s\n", operator);
  for(int i = 0; i < 3 && ((c = nextChar()) != EOF); i++) {
    printf("%i: %c\n", i, c);
    operator[i] = c;
  }
  if(operator[0] != SEPARATOR && operator[1] != SEPARATOR && (operator[2] == SEPARATOR || operator[2] == EOF || operator[2] == 0)) {
    printf("operator: %s\n", operator);
  } else {
    printf("not op\n");
  }
  return 0;
}

// decide where we're reading our source-code from
void pickInput(int argc) {
  if(argc == 1) {
    nextChar = getchar;
  } else {
    nextChar = nextCharArgc;
  }
}

int nextCharArgc() {
  static int positionArgumentsConsumed = 1;
  static int currentCharacterIndex = 0;

  printf("nextCharArgc\n");

  // argc 0 is program, so once we've consumed argc - 1 words we're done
  if(positionArgumentsConsumed == argc) {
    return EOF;
  }

  // pick the next character from our current argument
  char *argument = argv[positionArgumentsConsumed];
  char nextChar = argument[currentCharacterIndex];
  if(nextChar == 0) {
    positionArgumentsConsumed += 1;
    return nextCharArgc();
  }

  currentCharacterIndex += 1;
  return nextChar;
}

