#ifndef ERROR_H
#define ERROR_H

typedef enum {
    ERR_BOUNDS,
    ERR_TYPE,
    ERR_MEMORY,
    ERR_FILE,
    ERR_PARSE
} ErrorType;

void handle_error(ErrorType type, const char *msg);

#endif