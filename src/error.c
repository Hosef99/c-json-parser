#include "error.h"
#include <stdio.h>

void handle_error(ErrorType type, const char *msg) {
    const char *label;

    switch(type) {
        case ERR_BOUNDS: label = "Bounds"; break;
        case ERR_TYPE:   label = "Type"; break;
        case ERR_MEMORY: label = "Memory"; break;
        case ERR_PARSE:  label = "Parse"; break;
        default:         label = "Unknown";
    }

    fprintf(stderr, "[%s Error] %s\n", label, msg);
}