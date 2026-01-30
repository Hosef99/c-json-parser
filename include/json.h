#ifndef JSON_JSON_H
#define JSON_JSON_H

typedef enum {
    JSON_OBJECT,
    JSON_ARRAY,
    JSON_STRING,
    JSON_NUMBER,
    JSON_BOOLEAN,
    JSON_NULL
} JsonType;

typedef struct JsonValue JsonValue;
typedef struct JsonMember JsonMember;
typedef struct JsonTable JsonTable;

struct JsonTable {
    JsonMember *members;
    char **keys;
    size_t count;
    size_t capacity;
};

struct JsonValue {
    JsonType type;
    int line;
    union {
        JsonTable object;
        struct {
            JsonValue *values;
            size_t count;
            size_t capacity;
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
