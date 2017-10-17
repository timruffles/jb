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
# define debugf(format, ...) fprintf (stderr, "DEBUG:%d " format "\n", __LINE__, __VA_ARGS__)
# define debug(msg) fprintf (stderr, "DEBUG:%d " msg "\n", __LINE__)
# define assert(t, msg) if(!(t)) { fprintf (stderr, "ASSERT:%d " msg "\n", __LINE__); }


static char* input;
static uintmax_t inputLength;
// where are we in the input?
static uintmax_t inputOffset;

typedef char OperatorLiteral[3];
char OPERATOR_RE[] = "([oasc])([oce])( |$)";

void bug(bool, const char*);

char* stringJoin(uint8_t count, char* strings[], uintmax_t* size);

void output(char* string) {
  puts(string);
}

void outputc(char* string) {
  puts(string);
}



/**
 * Action constants
 */
enum ActionKind {
    OpenAction = 'o',
    CloseAction = 'c',
    EmptyAction = 'e'
};
int_fast32_t NotActionKind = -1;

enum ExpressionKind {
    EscapeToken,
    StringLiteral,
    NumericLiteral,
    BooleanLiteral,
    NullLiteral,
    ObjectPair = 'o',
    ArrayPair = 'a',
    StringPair = 's',
    ConcatPair = 'c'
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
  struct ObjectPairExpression objectPair;

  // all expression nodes have a Expression as their first member,
  // and we use this to get
  struct ObjectPairExpression all;
};


struct ParseResultS {
  bool success;
  char* errorMessage;
};


// we pass around nodes as pointers
typedef union Expression ExpressionNode;
typedef bool ParseResult;

static ParseResult expressionParse(ExpressionNode* node);

void objectEmpty() {
  output("{}");
}

static enum ExpressionKind getKind(ExpressionNode* node) {
  // TODO should this guard against null ptrs?
  return node->all.kind;
}

static bool isStringNode(ExpressionNode* node) {
  switch(getKind(node)) {
    case ObjectPair:
      return false;
    default:
      return true;
  }
}


static ParseResult objectPairParse(enum ActionKind action, ExpressionNode* result) {
  debug("object pair parse");
  switch(action) {
    // interesting note: as first line in case is an initialization (bool isKey =...)
    // we need to create a bracket statement to jump to
    case OpenAction: {
      bool isKey = true;
      while(true) {
        ExpressionNode* next = callocOne(sizeof (ExpressionNode));
        bool parsed = expressionParse(next);
        if(!parsed && !isKey) {
          // if we've parsed a key, we need a value too
          return false;
        }

        if(isKey && !isStringNode(next)) {
          return false;
        }

        // switch between
        isKey = !isKey;
      }

      return true;
    }

    case CloseAction:
      // FIXME
      result = NULL;
      return false;
    case EmptyAction: {
      const struct ObjectPairExpression pair = {
        .kind = ObjectPair,
      };

      result->objectPair = pair;
      return true;
    }

  }
}

static void advance(uint32_t n) {
  uintmax_t m = n + inputOffset ;
  inputOffset = m >= inputLength
    ? inputLength
    : m;
}

static bool isEof() {
  return inputOffset >= inputLength;
}

static enum ActionKind parseActionKind(char kind) {
  switch(kind) {
    case OpenAction:
    case CloseAction:
    case EmptyAction:
      return kind;
    default:
      return NotActionKind;
  }
}

static ParseResult pairParse(ExpressionNode* result) {
  static bool compiled = false;
  static regex_t operatorRe;
  // compile the regexp only once
  if(!compiled) {
    bug(!regcomp(&operatorRe, OPERATOR_RE, REG_EXTENDED | REG_ICASE), "One of the language regexps could not be compiled");
    compiled = true;
    debug("parsed operatorRe");
  }


  uint8_t matchCount = 3;
  regmatch_t operatorMatch[matchCount];
  int reResult = regexec(&operatorRe, &input[inputOffset], matchCount,
        operatorMatch, 0);
  debugf("result matching pair vs '%s' token '%i'", input, reResult);

  if(reResult != REG_NOMATCH) {
    advance(2);
    const enum ExpressionKind kind = input[operatorMatch[0].rm_so];
    const enum ActionKind action = parseActionKind(input[operatorMatch[1].rm_so]);
    if(action == NotActionKind) {
      return false;
    }

    debugf("parsed '%c' pair with '%c' action", kind, action);
    switch(kind) {
      case ObjectPair:
        return objectPairParse(action, result);
      default:
        return false;
    }
  }

  return false;
}

static ParseResult expressionParse(ExpressionNode* result) {
  if(isEof()) {
    return false;
  }

  debug("expressionParse()");
  if(pairParse(result)) {
    return true;
  }

  return false;
}



int main(int argc_, char* argv_[]) {
  // disable buffering for debuggability
  setbuf(stdout, NULL);

  // TODO - support stdin
  // ignore argv[0] - program name
  input = stringJoin(argc_ - 1, &argv_[1], &inputLength);
  debugf("parsing '%s' as jb", input);

  ExpressionNode* expression = callocOne(sizeof (union Expression));
  assert(expressionParse(expression), "Failed to parse");
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
char* stringJoin(uint8_t n, char* strings[], uintmax_t* size) {
  uintmax_t total = 0;
  for(uint8_t i = 0; i < n; i++) {
    total += (uintmax_t) strlen(strings[i]);
    // ensure we don't blow total
    assert(total <= INTMAX_MAX, "Input too large");
  }
  char* joined = calloc(total, sizeof (char));
  *size = total;
  for(uint8_t i = 0; i < n; i++) {
    strcat(joined, strings[i]);
  }
  return joined;
}


