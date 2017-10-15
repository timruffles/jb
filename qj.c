# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include <regex.h>
# include <string.h>

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

static const char INPUT_TOO_LARGE[] = "Input too large";
void bug(bool, const char*);

//// Take the next N characters. Will update TakeResult with
//// flags
//struct TakeResultT {
//  // was the request filled completely?
//  bool filled,
//  // how many taken
//  uint8_t n
//} TakeResult;
//
//char* take(uint8_t n) {
//}
//
//char* takeExact(char* string) {
//}
//
char* stringJoin(uint8_t count, char* strings[]);

void output(char* string) {
  puts(string);
}

void outputc(char* string) {
  puts(string);
}

char OPERATOR_RE[] = "([oasc])([oce])( |$)";

enum PairKind {
    ObjectPair,
    ArrayPair,
    StringPair,
    ConcatPair
};

enum PairAction {
  OpenPair,
  ClosePair,
  EmptyPair
};

static const char OBJECT_NESTING_INVALID[] = "Your object nesting is invalid";

void objectKind(enum PairAction action) {
  static uintmax_t nesting = 0;
  switch(action) {
    case OpenPair:
      nesting += 1;
      output("{");
      break;
    case ClosePair:
      if(nesting > 0) {
        nesting -= 1;
        output("}");
      } else {
        fputs(OBJECT_NESTING_INVALID, stderr);
        exit(1);
      }
      return;
    case EmptyPair:
      // TODO - should probably figure out if we're in an appropriate
      // context somehow - though this could be handled 'for free' if using
      // a recursive decent parser
      output("{}");
  }
}

int main(int argc_, char* argv_[]) {
  // TODO - support stdin
  // ignore argv[0] - program name
  char* input = stringJoin(argc_ - 1, &argv_[1]);
  puts(input);
  regex_t operatorRe;
  regmatch_t operatorMatch[1];
  bug(!regcomp(&operatorRe, OPERATOR_RE, REG_EXTENDED | REG_ICASE), "One of the language regexps could not be compiled");
  uint8_t matchCount = 1;
  if(regexec(&operatorRe, input, matchCount,
        operatorMatch, REG_EXTENDED | REG_ICASE)) {
    printf("operator '%.1s'", &input[operatorMatch[0].rm_so]);
    // NEXT - parse regexp match to pair kind + action, and pass to approprate operator 
  }
}


void assert(bool t, const char* msg) {
  if(!t) {
    fputs(msg, stderr);
    exit(1);
  }
}

// something that only happens if I screwed up
void bug(bool t, const char* msg) {
  if(!t) {
    fputs("Sorry, a programming error occured:", stderr);
    fputs(msg, stderr);
    exit(1);
  }
}

// returns pointer to new string which is concatentation of
// all other strings
char* stringJoin(uint8_t n, char* strings[]) {
  uintmax_t total = 0;
  for(uint8_t i = 0; i < n; i++) {
    total += (uintmax_t) strlen(strings[i]);
    // ensure we don't blow total
    assert(total <= INTMAX_MAX, &INPUT_TOO_LARGE[0]);
  }
  char* joined = calloc(total, sizeof (char));
  for(uint8_t i = 0; i < n; i++) {
    strcat(joined, strings[i]);
  }
  return joined;
}


int main2(int argc_, char* argv_[]) {
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

