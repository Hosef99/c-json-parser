#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "file.h"

int main() {
    char *source = read_file("test.json");
    char testing[] = "\"Hello\nWorld\"";

    init_lexer(source);
    Token token;
    Token tokens[100];
    int tok_count = 0;

    while (token.type != TOKEN_EOF) {
        token = scan_token();
        tokens[tok_count++] = token;
        print_token(token);
    }
    free(source);
}
