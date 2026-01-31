#ifndef JSON_ERROR_H
#define JSON_ERROR_H

#define HANDLE_ERROR(message) handle_error(message, __LINE__, __FILE__, __func__)

void handle_error(char *message, int line, char *file, const char *func);

#endif 
