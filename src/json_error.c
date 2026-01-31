#include <stdio.h>

#include "json_error.h"

void handle_error(char *message, int line, char *file, const char *func) {
    fprintf(stderr, "ERROR at %d, %s, %s: %s\n", line, file, func, message);
}
