#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "lexer.h"

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

void json_print(JsonValue *value, int indent) {
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
                json_print(&(value->as.object.members[i].value), indent + 2);
            }
            break;
        }
        case JSON_ARRAY: {
            printf("ARRAY  :\n");
            for (size_t i = 0; i < value->as.array.count; i++) {
                json_print(&(value->as.array.values[i]), indent + 1);
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
            return NULL; // ERROR
    }
}

static JsonValue *parse_object() {
    JsonValue *value = malloc(sizeof(JsonValue));
    advance();

    if (match(TOKEN_RBRACE)) {
        value->type = JSON_OBJECT;
        value->as.object.count = 0;
        value->as.object.members = NULL;
        return value;
    }

    size_t count = 0;
    size_t capacity = 4;
    JsonMember *members = malloc(capacity * sizeof(JsonMember));
    if (!members) {
        free(value);
        return NULL; // ERROR: memory allocation
    }

    for (;;) {
        if (count >= capacity) {
            size_t new_capacity = capacity * 2;
            JsonMember *temp_members = realloc(members, new_capacity * sizeof(JsonMember));

            if (!temp_members) {
                free(members);
                free(value);
                return NULL; // ERROR: memory allocation
            }

            capacity = new_capacity;
            members = temp_members;
        }

        if (!match(TOKEN_STRING)) {
            free(members);
            free(value);
            return NULL; // ERROR: Expect key to be string
        }
        JsonValue *key = parse_string();
        char *key_copy = strdup(key->as.string);

        if (!match(TOKEN_COLON)) {
            free(members);
            free(value);
            return NULL; // ERROR: Expect colon after key
        }
        advance();

        JsonValue *parsed = parse_value();
        if (parsed->type == JSON_ERROR) {
            free(members);
            free(value);            free(value);

            return NULL; // ERROR: error parsing
        }

        members[count].key = key_copy;
        members[count].value = *parsed;
        count++;

        if (match(TOKEN_COMMA)) {
            advance();
            continue;
        }

        if (match(TOKEN_RBRACE)) {
            advance();
            members = realloc(members, count * sizeof(JsonMember));
            if (!members) {
                free(members);
                free(value);
                return NULL; // ERROR: memory allocation
            }
            value->type = JSON_OBJECT;
            value->as.object.count = count;
            value->as.object.members = members;
            return value; 
        }

        // ERROR: expected RBRACE
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
        return NULL; // ERROR: memory allocation
    }

    for (;;) {
        if (count >= capacity) {
            size_t new_capacity = capacity * 2;
            JsonValue *temp_values = realloc(values, new_capacity * sizeof(JsonValue ));

            if (!temp_values) {
                free(values);
                free(value);
                return NULL; // ERROR: memory allocation
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

        // ERROR: expected RBRACK
        return NULL;
    }
    return NULL;
}

static JsonValue *parse_string() {
    JsonValue *value = malloc(sizeof(JsonValue));
    Token token = peek();
    value->type = JSON_STRING;
    value->as.string = strdup(token.string);
    advance();
    return value;
}

static JsonValue *parse_number() {
    JsonValue *value = malloc(sizeof(JsonValue));
    Token token = peek();
    value->type = JSON_NUMBER;
    value->as.number = strtod(token.string, NULL);
    advance();
    return value;
}

static JsonValue *parse_boolean() {
    JsonValue *value = malloc(sizeof(JsonValue));
    Token token = peek();
    value->type = JSON_BOOLEAN;
    value->as.boolean = token.type == TOKEN_TRUE;
    advance();
    return value;
}

static JsonValue *parse_null() {
    JsonValue *value = malloc(sizeof(JsonValue));
    value->type = JSON_NULL;
    advance();
    return value;
}

void json_free(JsonValue *value) {
    switch (value->type) {
        case JSON_OBJECT: {
            for (size_t i = 0; i < value->as.object.count; i++) {
                free(value->as.object.members[i].key);
            }
            free(value->as.object.members);
            break;
        }
        case JSON_ARRAY: {
            for (size_t i = 0; i < value->as.array.count; i++) json_free(&(value->as.array.values[i]));

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
