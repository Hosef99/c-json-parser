#include <stdio.h>
#include <stdlib.h>

#include "file.h"
#include "json.h"
#include "table.h"

void test_json() {
    const char *source = read_file("data/test/output_formatted.json");

    JsonValue *value = json_init(source);

    json_print(value);
}

void test_table() {
    JsonTable table;
    JsonValue *value = malloc(sizeof(JsonValue));
    value->type = JSON_NUMBER;
    value->as.number = 10.0;
    json_table_init(&table);
    json_table_set(&table, "key1", *value);
    
    JsonValue *get_value = json_table_get(&table, "key1");
}

int main() {
    test_json();

    return 0;
}
