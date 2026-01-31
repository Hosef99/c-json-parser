#include <stdio.h>
#include <string.h>

#include "lexer.h"
#include "string_builder.h"

char *start;
char *current;
int line;

void lexer_init(const char *source) {
    start = source;
    current = source;
    line = 1;
}

static bool is_digit(char c) {
    return (c >= '0' && c <= '9');
}

static bool is_alpha(char c) {
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_');
}

static bool is_at_end() {
    return current[0] == '\0';
}

static char peek() {
    return current[0];
}

static char peek_next() {
    if (is_at_end()) return '\0';
    return current[1];
}

static char advance() {
    return *current++;
}

static Token make_token(TokenType type, char *string) {
    Token token;
    token.type = type;
    token.string = string;
    token.line = line;
    return token;
}

static Token make_error_token(char *message) {
    Token token;
    token.type = TOKEN_ERROR;
    token.string = message;
    token.line = line;
    return token;
}

static Token string() {
    // account for escape characters
    while (peek() != '"' && !is_at_end()) {
        advance();
    }

    if (peek() != '"') return make_error_token("Unterminated String");

    advance();

    StringBuilder *sb = sb_create(16);
    sb_append_cstr_escaped_len(sb, start + 1, (size_t) (current-start - 2));
    Token token = make_token(TOKEN_STRING, sb_to_string(sb));
    sb_destroy(sb);
    return token;
}

static Token number() {
    while (is_digit(peek())) advance();

    if (peek() == '.' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) advance();
    }

    if (peek() == 'e' && is_digit(peek_next())) {
        advance();

        while (is_digit(peek())) advance();
    }

    StringBuilder *sb = sb_create(16);
    sb_append_cstr_len(sb, start, (size_t)(current - start));
    Token token = make_token(TOKEN_NUMBER, sb_to_string(sb));
    sb_destroy(sb);
    return token;
}

static Token identifier() {
    // only 3 identifiers really: "true", "false", and "null"
    while (is_alpha(peek()) || is_digit(peek())) advance();
    switch (start[0]) {
        case 't': {
            if (current - start == 4 && memcmp(start, "true", 4) == 0) {
                return make_token(TOKEN_TRUE, NULL); 
            } 
            break;
        }
        case 'f': {
            if (current - start == 5 && memcmp(start, "false", 5) == 0) {
                return make_token(TOKEN_FALSE, NULL);
            }
            break;
        }
        case 'n': {
            if (current - start == 4 && memcmp(start, "null", 4) == 0) {
                return make_token(TOKEN_NULL, NULL);
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
                advance();
                break;
            case '\n':
                advance();
                line++;
                break;
            default: 
                return;
        }
    }
}

Token scan_token() {
    skip_whitespace();
    start = current;

    if (is_at_end()) return make_token(TOKEN_EOF, NULL);
    char c = advance();

    if (is_digit(c)) return number();
    if (is_alpha(c)) return identifier();

    switch (c) {
        case '{': return make_token(TOKEN_LBRACE, NULL);
        case '}': return make_token(TOKEN_RBRACE, NULL);
        case '[': return make_token(TOKEN_LBRACK, NULL);
        case ']': return make_token(TOKEN_RBRACK, NULL);
        case ',': return make_token(TOKEN_COMMA, NULL);
        case ':': return make_token(TOKEN_COLON, NULL);
        case '"': return string();
    }

    return make_error_token("Unexpected character.");
}

void print_token(Token token) {
    printf("Type: %12s\nContent: %s\nLine: %d", token_type_to_string(token.type), token.string, token.line);
    printf("\n");
}
