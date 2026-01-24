#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "lexer.h"

#define JSON_VALUE_ERROR (JsonValue) {.type = JSON_ERROR}

typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL,
    JSON_ERROR
} JsonType;

typedef struct JsonValue JsonValue;
typedef struct JsonMember JsonMember;

struct JsonValue {
    JsonType type;
    union {
        struct {
            JsonMember *members; 
            size_t count;
        } object;
        struct {
            JsonValue *values;
            size_t count;
        } array;
        char *string;
        double number;
        int boolean;
    } as;
};

struct JsonMember {
    char *key;
    JsonValue value;
};

JsonValue json_parse(Token *tokens, size_t token_count);
void json_print(JsonValue value, int indent);

#endif
