# include<stdio.h>
# include<stdlib.h>
# include<stdint.h>
# include<string.h>


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

//void output(char* string) {
//    if(concat) {
//        puts(quote(string));
//    } else {
//        puts(string);
//    }
//}
//
//
//ConcatExpression concatExpression() {
//    if(!take("oe")) {
//        return;
//    }
//}


int main() {
}


