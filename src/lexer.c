#include <stdio.h>
#include <string.h>
#include "lexer.h"

typedef struct {
    const char *start;
    const char *current;
} Lexer;

Lexer lexer;

void init_lexer(const char *source) {
    lexer.start = source;
    lexer.current = source;
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static bool is_at_end() {
    return lexer.current[0] == '\0';
}

static char peek() {
    return lexer.current[0];
}

static char peek_next() {
    if (is_at_end()) return '\0';
    return lexer.current[1];
}

static char advance() {
    return *lexer.current++;
}

static Token make_token(TokenType type) {
    Token token;
    token.type = type;
    token.start = lexer.start;
    token.length = (int)(lexer.current - lexer.start);
    return token;
}

static Token make_error_token(char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.start = message;
    token.length = strlen(message);
    return token;
}

static Token string() {
    // account for escape characters
    while (peek() != '"' && !is_at_end()) {
        advance();
    }


    if (peek() != '"') return make_error_token("Unterminated String");

    advance();

    return make_token(TOKEN_STRING);
}

static Token number() {
    while (is_digit(peek())) advance();

    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) advance();
    }

    return make_token(TOKEN_NUMBER);
}

static Token identifier() {
    // only 3 identifiers really: "true", "false", and "null"
    while (is_alpha(peek()) || is_digit(peek())) advance();
    switch (lexer.start[0]) {
        case 't': {
            if (lexer.current - lexer.start == 4 && memcmp(lexer.start, "true", 4) == 0) {
                return make_token(TOKEN_TRUE);
            } 
            break;
        }
        case 'f': {
            if (lexer.current - lexer.start == 5 && memcmp(lexer.start, "false", 5) == 0) {
                return make_token(TOKEN_FALSE);
            }
            break;
        }
        case 'n': {
            if (lexer.current - lexer.start == 4 && memcmp(lexer.start, "null", 4) == 0) { return make_token(TOKEN_NULL);
            }
            break;
        }
        default: break;
    }
    return make_error_token("Unknown identifier");
}

void skip_whitespace() {
    for (;;) {
        switch (peek()) {
            case ' ':
            case '\t':
            case '\r':
            case '\n':
                advance();
                break;
            default: 
                return;
        }
    }
}

Token scan_token() {
    skip_whitespace();
    lexer.start = lexer.current;

    if (is_at_end()) return make_token(TOKEN_EOF);
    char c = advance();

    if (is_digit(c)) return number();
    if (is_alpha(c)) return identifier();

    switch (c) {
        case '{': return make_token(TOKEN_LBRACE);
        case '}': return make_token(TOKEN_RBRACE);
        case '[': return make_token(TOKEN_LBRACK);
        case ']': return make_token(TOKEN_RBRACK);
        case ',': return make_token(TOKEN_COMMA);
        case ':': return make_token(TOKEN_COLON);
        case '"': return string();
    }

    return make_error_token("Unexpected character.");
}

void lexer_test() {
    
}

void print_token(Token token) {
    printf("Type: %12s  Content: ", token_type_to_string(token.type));
    for (int i = 0; i < token.length; i++) {
        printf("%c", *(token.start + i));
    }
    printf("\n");
}
