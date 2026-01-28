#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "json.h"


JsonValue *json_init(const char *source) {
    lexer_init(source);

    size_t capacity = 8;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    
    while (tokens[count - 1].type != TOKEN_EOF) {
        if (count + 1 > capacity) {
            size_t new_capacity = capacity * 2;
            Token *temp_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!temp_tokens) {
                printf("Failed to allocate memory.\n");
                return NULL;
            }
            
            tokens = temp_tokens;
            capacity = new_capacity;
        }
        tokens[count] = scan_token();
        count++;
    }

    JsonValue *value = json_parse(tokens, count);

    return value;
}

JsonValue *json_array_get(JsonValue *value, size_t index) {
    if (index >= value->as.array.count) {
        handle_error(ERR_BOUNDS, "Array index out of bounds");
        return NULL;
}
    return &(value->as.array.values[index]);
}

JsonValue *json_object_get(JsonValue *value, char *key) {
    if (!json_is_object(value)) return NULL;
    for (size_t index = 0; index < value->as.object.count; index++) {
        if (strcmp(value->as.object.members[index].key, key) == 0) {
            return &(value->as.object.members[index].value);
        }
    }
    return NULL;
}

double json_as_number(JsonValue *value) {
   if (!json_is_number(value)) {
    handle_error(ERR_TYPE, "Expected number value");
    return -1;
} // ERROR
    return value->as.number;
}

char *json_as_string(JsonValue *value) {
    /* Be careful, this returns a reference, it will be destroyed when the json is destroyed. */
    if (!json_is_string(value)) {
    handle_error(ERR_TYPE, "Expected JSON string");
    return NULL;
} // ERROR
    return value->as.string;
}

char *json_copy_string(JsonValue *value) {
    if (value->type != JSON_STRING) {
handle_error(ERR_TYPE, "Expected JSON string for copy");
return NULL;
} // ERROR
    char *copied = strdup(value->as.string);
if (!copied) {
handle_error(ERR_MEMORY, "Failed to allocate string copy");
return NULL;
}


return copied;
}

int json_as_bool(JsonValue *value) {
    if (value->type != JSON_BOOLEAN) {
        handle_error(ERR_TYPE, "Expected JSON boolean");
        return -1;   // error indicator
    }
    return value->as.boolean;  // 0 or 1
}

int json_is_null(JsonValue *value) {
    return value->type == JSON_NULL;
}

int json_is_bool(JsonValue *value) {
    return value->type == JSON_BOOLEAN;
}

int json_is_string(JsonValue *value) {
    return value->type == JSON_STRING;
}

int json_is_number(JsonValue *value) {
    return value->type == JSON_NUMBER;
}

int json_is_array(JsonValue *value) {
    return value->type == JSON_ARRAY;
}

int json_is_object(JsonValue *value) {
    return value->type == JSON_OBJECT;
}

static void json_print_recursive(JsonValue *value, int indent) {
    for (int i = 0; i < indent; i++)
        printf("  ");
    switch (value->type) {
        case JSON_OBJECT: {
            printf("OBJECT :\n");
            for (size_t i = 0; i < value->as.object.count; i++) {
                for (int i = 0; i < indent + 1; i++)
                    printf("  ");
                printf("Key    : %s\n", value->as.object.members[i].key);
                for (int i = 0; i < indent + 1; i++)
                    printf("  ");
                printf("Value  :\n");
                json_print_recursive(&(value->as.object.members[i].value), indent + 2);
            }
            break;
        }
        case JSON_ARRAY: {
            printf("ARRAY  :\n");
            for (size_t i = 0; i < value->as.array.count; i++) {
                json_print_recursive(&(value->as.array.values[i]), indent + 1);
            }
            break;
        }
        case JSON_STRING:
            printf("STRING : %s\n", value->as.string);
            break;
        case JSON_NUMBER:
            printf("NUMBER : %g\n", value->as.number);
            break;
        case JSON_BOOLEAN:
            printf("BOOLEAN: %s\n", value->as.boolean ? "TRUE" : "FALSE");
            break;
        case JSON_NULL:
            printf("NULL:\n");
            break;
        default:
            printf("WHAT ARE YOU???\n");
            break;
    }
}

void json_print(JsonValue *value) {
    json_print_recursive(value, 0);
}

void json_destroy(JsonValue *value) {
    switch (value->type) {
        case JSON_OBJECT: {
            for (size_t i = 0; i < value->as.object.count; i++) {
                free(value->as.object.members[i].key);
            }
            free(value->as.object.members);
            break;
        }
        case JSON_ARRAY: {
            for (size_t i = 0; i < value->as.array.count; i++) json_destroy(&(value->as.array.values[i]));

            free(value->as.array.values);
            break;
        }
        case JSON_STRING: {
            free(value->as.string);
            break;
        }
        case JSON_NUMBER:
        case JSON_BOOLEAN:
        case JSON_NULL:
        case JSON_ERROR:
        default:
            break;
    }
    free(value);
}
