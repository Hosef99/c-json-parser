#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"
#include "table.h"
#include "json_error.h"

Token *tokens = NULL;
size_t token_count = 0;
size_t pos = 0;

static void advance() {
    if (pos < token_count) pos++;
}

static Token peek() {
    if (pos >= token_count) return TOKEN_EMPTY;
    return tokens[pos];
}

static int match(TokenType type) {
    Token token = peek();
    return token.type == type;
}

static JsonValue *parse_value();
static JsonValue *parse_object();
static JsonValue *parse_string();
static JsonValue *parse_number();
static JsonValue *parse_boolean();
static JsonValue *parse_null();
static JsonValue *parse_array();

JsonValue *json_parse(Token *input_tokens, size_t input_token_count) {
    token_count = input_token_count;
    tokens = input_tokens;
    pos = 0;

    JsonValue *result = parse_value();

    return result;
}

static JsonValue *parse_value() {
    Token curr_token = peek();
    switch (curr_token.type) {
        case TOKEN_STRING:
            return parse_string();
        case TOKEN_NUMBER:
            return parse_number();
        case TOKEN_TRUE:
        case TOKEN_FALSE:
            return parse_boolean();
        case TOKEN_NULL:
            return parse_null();
        case TOKEN_LBRACE:
            return parse_object();
        case TOKEN_LBRACK:
            return parse_array();
        default:
            HANDLE_ERROR("Unable to parse unknown token");
            return NULL;
    }
}

static JsonValue *parse_object() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) {
        HANDLE_ERROR("Memory allocation failed");
        return NULL;
    }

    advance();
    
    value->type = JSON_OBJECT;
    json_table_init(&value->as.object);

    if (match(TOKEN_RBRACE)) {
        return value;
    }

    for (;;) {
        if (!match(TOKEN_STRING)) {
            free(value);
            HANDLE_ERROR("Expecting a string key");
            return NULL;
        }
        JsonValue *key = parse_string();
        char *key_copy = strdup(key->as.string);

        if (!match(TOKEN_COLON)) {
            free(value);
            HANDLE_ERROR("Expect a ':' after key");
            return NULL;
        }
        advance();

        JsonValue *parsed = parse_value();
        if (parsed == NULL) {
            free(value);
            HANDLE_ERROR("Parsed value is NULL");
            return NULL;
        }

        json_table_set(&value->as.object, key_copy, *parsed);

        if (match(TOKEN_COMMA)) {
            advance();
            continue;
        }

        if (match(TOKEN_RBRACE)) {
            advance();
            return value; 
        }

        HANDLE_ERROR("Expect '}'");
        return NULL;
    }
    return NULL;
}

static JsonValue *parse_array() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) return NULL;
    advance();

    if (match(TOKEN_RBRACK)) {
        value->type = JSON_ARRAY;
        value->as.array.count = 0;
        value->as.array.values = NULL;
        return value;
    }

    size_t count = 0;
    size_t capacity = 4;
    JsonValue *values = malloc(capacity * sizeof(JsonValue));
    if (!values) {
        free(value);
        HANDLE_ERROR("Failed to allocate memory");
        return NULL;
    }

    for (;;) {
        if (count >= capacity) {
            size_t new_capacity = capacity * 2;
            JsonValue *temp_values = realloc(values, new_capacity * sizeof(JsonValue ));

            if (!temp_values) {
                free(values);
                free(value);
                HANDLE_ERROR("Failed to allocate memory");
                return NULL;
            }

            capacity = new_capacity;
            values = temp_values;
        }

        JsonValue *parsed = parse_value();
        values[count] = *parsed;
        count++;

        if (match(TOKEN_COMMA)) {
            advance();
            continue;
        }

        if (match(TOKEN_RBRACK)) {
            values = realloc(values, count * sizeof(JsonValue));
            advance();
            value->type = JSON_ARRAY;
            value->as.array.count = count;
            value->as.array.values = values;
            return value; 
        }

        HANDLE_ERROR("Expect '}'");
        return NULL;
    }
    return NULL;
}

static JsonValue *parse_string() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) {
        HANDLE_ERROR("Failed to allocate memory");
        return NULL;
    }
    Token token = peek();
    value->type = JSON_STRING;
    value->as.string = strdup(token.string);
    if (!value->as.string) {
        HANDLE_ERROR("Failed to duplicate string");
    }
    advance();
    return value;
}

static JsonValue *parse_number() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) {
        HANDLE_ERROR("Failed to allocate memory");
        return NULL;
    }
    Token token = peek();
    value->type = JSON_NUMBER;
    value->as.number = strtod(token.string, NULL);
    advance();
    return value;
}

static JsonValue *parse_boolean() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) {
        HANDLE_ERROR("Failed to allocate memory");
        return NULL;
    }
    Token token = peek();
    value->type = JSON_BOOLEAN;
    value->as.boolean = token.type == TOKEN_TRUE;
    advance();
    return value;
}

static JsonValue *parse_null() {
    JsonValue *value = malloc(sizeof(JsonValue));
    if (!value) {
        HANDLE_ERROR("Failed to allocate memory");
        return NULL;
    }
    value->type = JSON_NULL;
    advance();
    return value;
}

