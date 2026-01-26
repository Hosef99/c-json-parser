#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "table.h"
#include "parser.h"

#define TABLE_MAX_LOAD 0.75

static uint32_t hash_string(const char *key, int length) {
    uint32_t hash = 2166136261u;
    for (int i = 0; i < length; i++) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619;
    }
    return hash;
}

static JsonMember *find_member(JsonMember *members, size_t capacity, char *key) {
    size_t length = strlen(key);
    uint32_t index = hash_string(key, length) % capacity;
    
    JsonMember *tombstone = NULL;

    while (1) {
        JsonMember *member = &members[index];
        if (member->key == NULL) {
            if (member->value.type == JSON_NULL) {
                return tombstone != NULL ? tombstone : member;
            } else {
                if (tombstone == NULL) tombstone = member;
            }
        }

        if (strcmp(member->key, key) == 0) {
            return member;
        }

        index = (index + 1) % capacity;
    }
}

static void adjust_capacity(Table *table, size_t capacity) {
    JsonMember *members = malloc(sizeof(JsonMember));
    for (size_t i = 0; i < capacity; i++) {
        members[i].key = NULL;
        members[i].value.type = JSON_NULL;
    }

    table->count = 0;
    for (size_t i = 0; i < table->capacity; i++) {
        JsonMember *member = &table->members[i];
        if (member->key == NULL) continue;

        JsonMember *dest = find_member(member, capacity, member->key);
        dest->key = member->key;
        dest->value = member->value;
        table->count++;
    }

    free(table->members);

    table->members = members;
    table->capacity = capacity;
}

void json_table_init(Table *table) { 
    table->count = 0;
    table->capacity = 0;
    table->members = NULL;
}

void json_table_free(Table *table) {
    table->count = 0;
    table->capacity = 0;
    free(table->members);
}

bool json_table_get(Table *table, char *key, JsonValue *value) { 
    if (table->count == 0) return false;

    JsonMember *member = find_member(table->members, table->capacity, key);
    if (member->key == NULL) return false;

    *value = member->value;
    return true;
}

bool json_table_set(Table *table, char *key, JsonValue value) { 
    if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
       size_t capacity = table->capacity ? table->capacity * 2 : 8;
        adjust_capacity(table, capacity);
    }
    JsonMember *member = find_member(table->members, table->capacity, key);
    bool is_new_key = member->key == NULL;

    if (is_new_key && member->value.type == JSON_NULL) table->count++;

    member->key = key;
    member->value = value;
    return is_new_key;
}

bool json_table_delete(Table *table, char *key) { 
    if (table->count == 0) return false;

    JsonMember *member = find_member(table->members, table->capacity, key);
    if (member->key == NULL) return false;

    member->key = NULL;
    member->value.type = JSON_NULL;
    return true;
}

void json_table_copy(Table *from, Table *to) {

}
