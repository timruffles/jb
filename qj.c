# include <stdio.h>
# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>
# include <regex.h>
# include <string.h>

# define EOS '\0'
// variables are initialized to 0
# define UNINITIALIZED 0;
# define callocOne(A) calloc(1, A);


static char* input;


typedef char OperatorLiteral[3];
char OPERATOR_RE[] = "([oasc])([oce])( |$)";

static const char INPUT_TOO_LARGE[] = "Input too large";
void bug(bool, const char*);

char* stringJoin(uint8_t count, char* strings[]);

void output(char* string) {
  puts(string);
}

void outputc(char* string) {
  puts(string);
}

void assert(bool t, const char* msg) {
  if(!t) {
    fputs(msg, stderr);
    exit(1);
  }
}

/**
 * Object pair constants
 */
enum ObjectKind {
    ObjectPairKind = 'o',
};

/**
 * Action constants
 */
enum ActionKind {
    OpenAction = 'o',
    CloseAction = 'c',
    EmptyAction = 'e'
};

enum ExpressionKind {
    EscapeToken,
    StringLiteral,
    NumericLiteral,
    BooleanLiteral,
    NullLiteral,
    ObjectPair,
    ArrayPair,
    StringPair,
    ConcatPair
};

struct ObjectPairExpression {
  enum ExpressionKind kind;
  union Expression* pairs[];
};


/**
 * Expressions are the building-blocks of the language, and
 * each evaluates to a value.
 */
union Expression {
  enum ExpressionKind kind;
};



static const char OBJECT_NESTING_INVALID[] = "Your object nesting is invalid";


void objectEmpty() {
  output("{}");
}

typedef union Expression* ParseResult;

static struct ObjectPairExpression* objectPairParse(enum ActionKind action, ParseResult result) {
  static uintmax_t nesting = 0;
  switch(action) {
    case OpenAction:
      nesting += 1;
      output("{");
      break;
    case CloseAction:
      if(nesting > 0) {
        nesting -= 1;
        output("}");
      } else {
        fputs(OBJECT_NESTING_INVALID, stderr);
        exit(1);
      }
      return;
    case EmptyAction:
      // TODO - should probably figure out if we're in an appropriate
      // context somehow - though this could be handled 'for free' if using
      // a recursive decent parser
      output("{}");
  }
}


static ParseResult pairParse(ParseResult result) {
  static bool compiled = false;
  static regex_t operatorRe;
  // compile the regexp only once
  if(!compiled) {
    bug(!regcomp(&operatorRe, OPERATOR_RE, REG_EXTENDED | REG_ICASE), "One of the language regexps could not be compiled");
    compiled = true;
  }

  regmatch_t operatorMatch[1];
  uint8_t matchCount = 1;
  if(regexec(&operatorRe, input, matchCount,
        operatorMatch, REG_EXTENDED | REG_ICASE)) {
    const enum ObjectCharacter kind = input[operatorMatch[0].rm_so];
    const enum ActionCharacter action = input[operatorMatch[1].rm_so];
    switch(kind) {
      case ObjectPairKindCharacter:
        return objectPairParse(action, result);
    }
  }

  return NULL;
}

static ParseResult expressionParse() {
  ParseResult expression = callocOne(sizeof (union Expression));

  if((expression = pairParse(expression)) != NULL) {
    return expression;
  }

  return NULL;
}



int main(int argc_, char* argv_[]) {
  // TODO - support stdin
  // ignore argv[0] - program name
  input = stringJoin(argc_ - 1, &argv_[1]);
  puts(input);

  assert(expressionParse() == NULL, "Failed to parse");
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


