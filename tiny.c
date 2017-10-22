# include<stdio.h>
# include<stdlib.h>
# include<stdint.h>
# include<string.h>
# include<assert.h>
# include<stdbool.h>

// size for string + \0 - if we need a string for "hello", we'd end up
// with 6 bytes. calloc allocs 0 so after copying it in it's terminated
#define callocString(size) calloc(size, sizeof (char) + 1);

const char ConcatOpen[] = "co";
const char ConcatClose[] = "cc";
const char EscapeToken[] = "et";

uintmax_t concatDepth = 0;

struct ConcatOuput {
    char* content;
    // how many bytes of content are filled
    uintmax_t length;
    // how much room we have in content
    uintmax_t size;
    struct ConcatOuput* parent;
};

// our concat stack
struct ConcatOuput* currentConcat;

// just output verbatim
void output(char*);
// output a JSON string
void outputString(char*);

char** programTokens;
int programLength;
int pc = 0;

char* getToken() {
    if(pc == programLength) {
        return NULL;
    } else {
        char* token = programTokens[pc];
        pc += 1;
        //printf("considering '%s'\n", token);
        return token;
    }
}

struct QuoteResult {
    int length;
    char* string;
};

/* replaces all " with \" and \ with \\ */
struct QuoteResult quote(char* string) {
    int length = strlen(string);
    // at most, we'll replace every character
    char* replaced = callocString(length * 2);
    int o = 0;
    for(int i = 0; i < length; i++) {
        char c = string[i];
        if(c == '\\') {
            replaced[o] = replaced[o + 1] = '\\';
            o += 1;
        } else if(c == '"') {
            replaced[o] = '\\';
            replaced[o + 1] = '"';
            o += 1;
        } else {
            replaced[o] = string[i];
        }
        o += 1;
    }
    printf("got to %i vs %i '%s'\n", o, length, replaced);
    replaced[o] = '\0';
    return (struct QuoteResult) {
        .length = length,
        .string = replaced,
    };
}

void concatClose() {
    assert(concatDepth > 0);
    concatDepth -= 1;
    struct ConcatOuput* co = currentConcat;
    currentConcat = currentConcat->parent;
    printf("outputting '%s'\n", co->content);
    outputString(co->content);
    free(co->content);
    free(co);
}


char* escapeTokens[] = {
    "nl", "\n",
    "quote", "\"",
};
// TODO more elegant way to do this
uintmax_t EscapeTokensLength = 2 * 2;

void escapeToken() {
    char* token = getToken();
    // no token to escape
    assert(token != NULL);
    for(uintmax_t i = 0; i < EscapeTokensLength; i+=2) {
        char* et = escapeTokens[i];
        if(strcmp(et, token) == 0) {
            outputString(escapeTokens[i + 1]);
        }
    }
}

void concatOpen() {
    concatDepth += 1;
    struct ConcatOuput* created = calloc(1, sizeof (struct ConcatOuput));
    char* content = callocString(512);
    *created = (struct ConcatOuput){
        .content = content,
        .size = 0,
        .length = 0,
        .parent = NULL
    };
    if(currentConcat != NULL) {
        created->parent = currentConcat;
    }
    currentConcat = created;
}

void outputString(char* string) {
    struct QuoteResult inner = quote(string);
    int len = inner.length;
    char* quoted = callocString(len + 2);
    strlcpy(&quoted[1], inner.string, len + 1);
    quoted[0] = quoted[len + 1] = '\"';
    free(inner.string);
    output(quoted);
}

void output(char* string) {
    if(currentConcat == NULL) {
        // double size of concat buffer each time
        puts(string);
    } else {
        int len = strlen(string);
        // ensure we've enough space
        while(currentConcat->length + len >= currentConcat->size - 1) {
            // TODO overflow
            int newSize = currentConcat->size * 2;
            char* newContent = callocString(newSize);
            strncpy(currentConcat->content, newContent, currentConcat->length);
            currentConcat->size = newSize;
            currentConcat->content = newContent;
        }
        strncat(currentConcat->content, string, len);
    }
}

bool expression() {
    char* token = getToken();
    if(token != NULL) {
        if(strcmp(token, ConcatOpen) == 0) {
            //puts("co");
            concatOpen();
        } else if(strcmp(token, ConcatClose) == 0) {
            //puts("cc");
            concatClose();
        } else if(strcmp(token, EscapeToken) == 0) {
            escapeToken();
        } else {
            outputString(token);
        }
        return true;
    }
    return false;
}

int main(int argc, char* argv[]) {
    printf("program of length '%i'\n", argc);
    programLength = argc - 1;
    programTokens = &argv[1];
    while(expression())
        ;
}


