#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#ifndef PARSER_H
#define PARSER_H

typedef enum {
    SEMICOL,
    AND,
    OR,
    PARAL,
    PIPE,
    END
} separator_t;

typedef enum {
    INPUT,
    OUTPUT,
    OUTPUT_APPEND,
    ERROR,
    ERROR_APPEND
} redirection_t;

int trim(char* str);
int clean(char* str);
size_t split_str(char* str, char* tab[], size_t max_size);
int command(char* tab[], separator_t* sep);
int redirection(char* tab[], redirection_t* redir);
int split_command(char* tab[], separator_t* sep);

#endif

