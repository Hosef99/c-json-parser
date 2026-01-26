#include <stdio.h>
#include <stdlib.h>

#include "table.h"
#include "lexer.h"
#include "file.h"
#include "parser.h"
#include "string_builder.h"

int main() {
    char *source = read_file("data/test/object.json");

    init_lexer(source);

    Token *tokens = malloc(3000 * sizeof(Token));
    size_t token_count = 0;

    int loop_count = 3000;

    while (tokens[token_count - 1].type != TOKEN_EOF && loop_count--) {
        tokens[token_count] = scan_token();
        token_count++;
    }

    JsonValue *value = json_parse(tokens, token_count);

    json_print(value, 0);

    json_free(value);

    free(tokens);
}
