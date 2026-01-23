#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "file.h"
#include "string_builder.h"

int main() {
    char testing[] = "123.4556\"Hello\\nWorld\"";

    init_lexer(testing);
    Token token;

    while (token.type != TOKEN_EOF) {
        token = scan_token();
        print_token(token);
    }
}
