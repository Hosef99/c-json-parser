#include <stdio.h>
#include <stdlib.h>
#include "error.h"

char* read_file(const char* path) {
    FILE* file = fopen(path, "rb");
    if (!file) {
        handle_error(ERR_FILE, "Failed to open file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    if (size < 0) {
        handle_error(ERR_FILE, "Failed to get file size");
        fclose(file);
        return NULL;
    }
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        handle_error(ERR_MEMORY, "Failed to allocate file buffer");
        fclose(file);
        return NULL;
    }

    if (fread(buffer, 1, size, file) != (size_t)size) {
        handle_error(ERR_FILE, "Failed to read file");
        free(buffer);
        fclose(file);
        return NULL;
    }

    buffer[size] = '\0';
    fclose(file);
    return buffer;
}