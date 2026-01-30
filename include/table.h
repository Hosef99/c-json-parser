#ifndef JSON_TABLE_H
#define JSON_TABLE_H

#include "parser.h"
#include "json.h"

void json_table_init(JsonTable *table);
void json_table_free(JsonTable *table);
JsonValue *json_table_get(JsonTable *table, char *key);
bool json_table_set(JsonTable *table, char *key, JsonValue value);
bool json_table_delete(JsonTable *table, char *key); 
void json_table_copy(JsonTable *from, JsonTable *to);

#endif
