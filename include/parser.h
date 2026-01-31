#ifndef JSON_PARSER_H
#define JSON_PARSER_H

#include "lexer.h"
#include "json.h"

JsonValue *json_parse(Token *tokens, size_t token_count);

#endif
