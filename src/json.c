#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"

#include "json.h"
#include "table.h"
#include "json_error.h"


JsonValue *json_init(const char *source) {
    lexer_init(source);

    size_t capacity = 8;
    size_t count = 0;
    Token *tokens = malloc(capacity * sizeof(Token));
    
    if (!tokens) {
        HANDLE_ERROR("Failed to allocate memory.");
    }
    
    while (tokens[count - 1].type != TOKEN_EOF) {
        if (count + 1 > capacity) {
            size_t new_capacity = capacity * 2;
            Token *temp_tokens = realloc(tokens, new_capacity * sizeof(Token));
            if (!temp_tokens) {
                HANDLE_ERROR("Failed to allocate memory.");
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
        HANDLE_ERROR("index out of bounds.");
        return NULL;
    }
    return &(value->as.array.values[index]);
}

JsonValue *json_object_get(JsonValue *value, char *key) {
    if (!json_is_object(value)) {
        HANDLE_ERROR("value is not object.");
        return NULL;
    }
    JsonValue *result = json_table_get(&value->as.object, key);

    return result;
}

double json_as_number(JsonValue *value) {
    if (!json_is_number(value)) {
        HANDLE_ERROR("value is not number.");
        return -1;
    }
    return value->as.number;
}

char *json_as_string(JsonValue *value) {
    /* Be careful, this returns a reference, it will be destroyed when the json is destroyed. */
    if (!json_is_string(value)) {
        HANDLE_ERROR("value is not string");
        return NULL;
    }
    return value->as.string;
}

char *json_copy_string(JsonValue *value) {
    if (!json_is_string(value)) {
        HANDLE_ERROR("value is not string");
        return NULL;
    }
    char *copied = strdup(value->as.string);
    return copied;
}

int json_as_bool(JsonValue *value) {
    if (!json_is_bool(value)) {
        HANDLE_ERROR("value is not bool");
        return false;
    }
    return value->as.boolean;
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
    if (!value) {
        HANDLE_ERROR("value is NULL");
        return;
    }

    for (int i = 0; i < indent; i++)
        printf("  ");
    switch (value->type) {
        case JSON_OBJECT: {
            printf("OBJECT :\n");
            for (size_t i = 0; i < value->as.object.count; i++) {
                char *key = value->as.object.keys[i];
                JsonValue *member_value = json_table_get(&value->as.object, key);
                for (int i = 0; i < indent + 1; i++)
                    printf("  ");
                printf("Key    : %s\n", value->as.object.keys[i]);
                for (int i = 0; i < indent + 1; i++)
                    printf("  ");
                printf("Value  :\n");
                json_print_recursive(member_value, indent + 2);
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
            HANDLE_ERROR("Value type unknown");
            break;
    }
}

void json_print(JsonValue *value) {
    if (!value) {
        HANDLE_ERROR("value is NULL");
        return;
    }
    json_print_recursive(value, 0);
}

static void json_write_compact_recursive(JsonValue *value, FILE *file) {
    switch (value->type) {
        case JSON_OBJECT: {
            fprintf(file, "{");
            size_t count = value->as.object.count;
            for (size_t index = 0; index < count; index++) {
                char *key = value->as.object.keys[index];
                JsonValue *member_value = json_table_get(&value->as.object, key);
                fprintf(file, "\"%s\":", key);
                json_write_compact_recursive(member_value, file);
                if (index != count - 1) {
                    fprintf(file, ",");
                }
            }
            fprintf(file, "}");
            break;
        }
        case JSON_ARRAY: {
            fprintf(file, "[");
            size_t count = value->as.array.count;
            for (size_t index = 0; index < count; index++) {
                json_write_compact_recursive(&value->as.array.values[index], file);
                if (index != count - 1) {
                    fprintf(file, ",");
                }
            }
            fprintf(file, "]");
            break;
        }
        case JSON_STRING: {
            fprintf(file, "\"%s\"", value->as.string);
            break;
        }
        case JSON_NUMBER: {
            fprintf(file, "%g", value->as.number);
            break;
        }
        case JSON_BOOLEAN: {
            fprintf(file, "%s", value->as.boolean ? "true" : "false");
            break;
        }
        case JSON_NULL: {
            fprintf(file, "null");
            break;
        }
        default:
            HANDLE_ERROR("Value type unknown");
            return;
    }
}

void json_write_compact(JsonValue *value, const char *filepath) {
    if (!value) {
        HANDLE_ERROR("value is NULL");
        return;
    }
    FILE *file = fopen(filepath, "w");
    if (!file) {
        HANDLE_ERROR("Value type unknown");
        return;
    }

    json_write_compact_recursive(value, file);
}

static void json_write_recursive(JsonValue *value, FILE *file, int indent) {
    switch (value->type) {
        case JSON_OBJECT: {
            fprintf(file, "{\n");
            size_t count = value->as.object.count;
            for (size_t index = 0; index < count; index++) {
                char *key = value->as.object.keys[index];
                JsonValue *member_value = json_table_get(&value->as.object, key);
                for (int i = 0; i < indent + 1; i++) 
                    fprintf(file, "    ");
                fprintf(file, "\"%s\": ", key);
                json_write_recursive(member_value, file, indent + 1);
                if (index != count - 1) 
                    fprintf(file, ",\n");
            }
            fprintf(file, "\n");
            for (int i = 0; i < indent; i++)
                fprintf(file, "    ");
            fprintf(file, "}");
            break;
        }
        case JSON_ARRAY: {
            fprintf(file, "[\n");
            size_t count = value->as.array.count;
            for (size_t index = 0; index < count; index++) {
                for (int i = 0; i < indent + 1; i++)
                    fprintf(file, "    ");
                json_write_recursive(&value->as.array.values[index], file, indent + 1);
                if (index != count - 1) 
                    fprintf(file, ",\n");
            }
            fprintf(file, "\n");
            for (int i = 0; i < indent; i++)
                fprintf(file, "    ");
            fprintf(file, "]");
            break;
        }
        case JSON_STRING: {
            fprintf(file, "\"%s\"", value->as.string);
            break;
        }
        case JSON_NUMBER: {
            fprintf(file, "%g", value->as.number);
            break;
        }
        case JSON_BOOLEAN: {
            fprintf(file, "%s", value->as.boolean ? "true" : "false");
            break;
        }
        case JSON_NULL: {
            fprintf(file, "null");
            break;
        }
        default:
            HANDLE_ERROR("Value type unknown");
            return;
    }
}

void json_write(JsonValue *value, const char *filepath) {
    if (!value) {
        HANDLE_ERROR("value is NULL");
        return;
    }
    FILE *file = fopen(filepath, "w");
    if (!file) {
        HANDLE_ERROR("Value type unknown");
        return;
    }

    json_write_recursive(value, file, 0);
}

void json_destroy(JsonValue *value) {
    if (!value) {
        HANDLE_ERROR("value is NULL");
        return;
    }
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
        default:
            break;
    }
    free(value);
}
