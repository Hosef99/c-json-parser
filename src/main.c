#include <stdio.h>
#include <stdlib.h>

#include "json.h"
#include "table.h"
#include "lexer.h"
#include "file.h"
#include "parser.h"
#include "string_builder.h"

int main() {
    const char *source = read_file("data/test/object.json");

    JsonValue *value = json_init(source);

    json_print(value);

    return 0;
}
