#include <stdio.h>
#include <stdlib.h>

#include "json.h"
#include "table.h"
#include "lexer.h"
#include "file.h"
#include "parser.h"
#include "string_builder.h"
#include "error.h"

int main() {
    const char *source = read_file("data/test/object.json");
    if (!source) {
        handle_error(ERR_FILE, "Failed to load JSON file");
        return EXIT_FAILURE;
    }

    JsonValue *value = json_init(source);
    if (!value) {
        handle_error(ERR_PARSE, "Failed to parse JSON");
        free((void*)source);
        return EXIT_FAILURE;
    }

    json_print(value);

    json_destroy(value);
    free((void*)source);

    return EXIT_SUCCESS;
}