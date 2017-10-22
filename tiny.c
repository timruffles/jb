# include<stdio.h>
# include<stdlib.h>
# include<stdint.h>
# include<string.h>
# include<assert.h>
# include<stdbool.h>

#define callocString(size) calloc(size, sizeof (char));

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

void output(char*);

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

/* replaces all " with \" and \ with \\ */
char* quote(char* string) {
    int length = strlen(string);
    // at most, we'll replace every character
    char* replaced = calloc(length * 2, sizeof (char));
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
    return replaced;
}

void concatClose() {
    assert(concatDepth > 0);
    concatDepth -= 1;
    struct ConcatOuput* co = currentConcat;
    currentConcat = currentConcat->parent;
    output(co->content);
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
            output(escapeTokens[i + 1]);
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
            output(token);
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


