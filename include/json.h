#ifndef JSON_JSON_H
#define JSON_JSON_H

#include "parser.h"
#include "table.h"

JsonValue *json_array_get(JsonValue *value, size_t index);
JsonValue *json_object_get(JsonValue *value, char *key);

double json_as_number(JsonValue *value);
char *json_as_string(JsonValue *value);
char *json_copy_string(JsonValue *value);
int json_as_bool(JsonValue *value);

int json_is_null(JsonValue *value);
int json_is_bool(JsonValue *value);
int json_is_string(JsonValue *value);
int json_is_number(JsonValue *value);
int json_is_array(JsonValue *value);
int json_is_object(JsonValue *value);

JsonValue *json_init(const char *source);
void json_print(JsonValue *value);
void json_destroy(JsonValue *value);

#endif
