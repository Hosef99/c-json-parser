#ifndef JSON_LEXER_H
#define JSON_LEXER_H

#define TOKEN_EMPTY (Token) {.type = TOKEN_ERROR, .string = "Empty"};

typedef enum {
    TOKEN_STRING,
    TOKEN_NUMBER,
    TOKEN_TRUE,
    TOKEN_FALSE,
    TOKEN_NULL,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACK,
    TOKEN_RBRACK,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_ERROR,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    const char *string;
} Token;

void init_lexer(const char *source);
Token scan_token();
void lexer_test();
void print_token(Token token);

static inline char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_STRING: return "TOKEN_STRING";
        case TOKEN_NUMBER: return "TOKEN_NUMBER";
        case TOKEN_TRUE: return "TOKEN_TRUE";
        case TOKEN_FALSE: return "TOKEN_FALSE";
        case TOKEN_NULL: return "TOKEN_NULL";
        case TOKEN_LBRACE: return "TOKEN_LBRACE";
        case TOKEN_RBRACE: return "TOKEN_RBRACE";
        case TOKEN_LBRACK: return "TOKEN_LBRACK";
        case TOKEN_RBRACK: return "TOKEN_RBRACK";
        case TOKEN_COMMA: return "TOKEN_COMMA";
        case TOKEN_COLON: return "TOKEN_COLON";
        case TOKEN_ERROR: return "TOKEN_ERROR";
        case TOKEN_EOF: return "TOKEN_EOF";
        default: return "TOKEN_UNDEFINED";
    }
}

#endif
