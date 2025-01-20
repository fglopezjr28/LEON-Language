#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define INITIAL_BUFFER_SIZE 2000

typedef struct {
    char *data;
    int size;
    int index;
} DynamicBuffer;

void initBuffer(DynamicBuffer *buffer, int initialSize) {
    buffer->data = (char *)malloc(initialSize);
    buffer->size = initialSize;
    buffer->index = 0;
    if (!buffer->data) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
}

void appendToBuffer(DynamicBuffer *buffer, const char *text) {
    int textLength = strlen(text);

    if (buffer->index + textLength >= buffer->size) {
        buffer->size *= 2;
        buffer->data = (char *)realloc(buffer->data, buffer->size);
        if (!buffer->data) {
            fprintf(stderr, "Memory allocation failed during resizing.\n");
            exit(EXIT_FAILURE);
        }
    }

    strcpy(buffer->data + buffer->index, text);
    buffer->index += textLength;
}

int freeBuffer(DynamicBuffer *buffer) {
    free(buffer->data);
    return 0;
}

void classifyAndAppend(const char *token, int lineNumber, DynamicBuffer *buffer) {
    char temp[128];

    switch (token[0]) {
        case '+': case '-': case '*': case '%': case '^': // Arithmetic Operators
            if (token[1] == '\0')
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "ARITHMETIC_OP", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '=': // Assignment Operator or Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", "==", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "ASSIGNMENT_OP", "=", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '!': // Relational Operator
            if (token[1] == '=') {
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", "!=", lineNumber);
                appendToBuffer(buffer, temp);
            } else if (token[1] == '\0') {
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "LOGICAL_OP", "!", lineNumber);
                appendToBuffer(buffer, temp);
            }
            break;
        case '>': // Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", ">=", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", ">", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '<': // Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", "<=", lineNumber);
            else 
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "RELATIONAL_OP", "<", lineNumber);
            appendToBuffer(buffer, temp); 
            break;
        case '&': // Logical Operator
            if (token[1] == '&')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "LOGICAL_OP", "&&", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '|': // Logical Operator
            if (token[1] == '|')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "LOGICAL_OP", "||", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '/': // Arithmetic Operator or Delimiter
            if (token[1] == '/')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "LINE_TERMINATOR", "//", lineNumber);
            else if (token[1] == '<')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "ARITHMETIC_OP", "/<", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "ARITHMETIC_OP", "/", lineNumber);
                appendToBuffer(buffer, temp);
            break;
        case '(': case ')': case '{': case '}': case '[': case ']': case ',': // Delimiters
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "DELIMETER", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '$': // Comment
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "COMMENT", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;

        // Check for 'and' and 'alt'
        case 'a': // Handles words starting with 'a'
            switch (token[1]) {
                case 'n': // Check if the second character is 'n'
                    switch (token[2]) {
                        case 'd': // Check if the third character is 'd'
                            switch (token[3]) {
                                case '\0': // End of string (exactly "and")
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "NOISE_WORD", "and", lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                                default: // More characters after "and"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Third character is not 'd' (e.g., "anf")
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                case 'l': // Check if the second character is 'l' (for "alt")
                    switch (token[2]) {
                        case 't': // Check if the third character is 't'
                            switch (token[3]) {
                                case '\0': // End of string (exactly "alt")
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "KEYWORD", "alt", lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                                default: // More characters after "alt"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Third character is not 't' (e.g., "alx")
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                default: // Second character is neither 'n' nor 'l'
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        // Check for 'binar' and 'bind'
        case 'b': // Handles words starting with 'b'
            switch (token[1]) {
                case 'i': // Check for "bi"
                    switch (token[2]) {
                        case 'n': // Check for "bin"
                            switch (token[3]) {
                                case 'a': // Check for "bina"
                                    switch (token[4]) {
                                        case 'r': // Check for "binar"
                                            if (token[5] == '\0') { // End of string
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "binar", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "binar"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "bina"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;

                                case 'd': // Check for "bind"
                                    if (token[4] == '\0') { // End of string
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "bind", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "bind"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;

                                default: // Invalid after "bin"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;

                        default: // Invalid after "bi"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                default: // Invalid after 'b'
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;


        // Check for 'const' and 'craft'
        case 'c': // Handles words starting with 'c'
            switch (token[1]) {
                case 'o': // Check for "co"
                    switch (token[2]) {
                        case 'n': // Check for "con"
                            switch (token[3]) {
                                case 's': // Check for "cons"
                                    switch (token[4]) {
                                        case 't': // Check for "const"
                                            if (token[5] == '\0') { // End of string confirms "const"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "RESERVED_WORD", "const", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "const"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "cons"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "con"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "co"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                case 'r': // Check for "cr"
                    switch (token[2]) {
                        case 'a': // Check for "cra"
                            switch (token[3]) {
                                case 'f': // Check for "craf"
                                    switch (token[4]) {
                                        case 't': // Check for "craft"
                                            if (token[5] == '\0') { // End of string confirms "craft"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "craft", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "craft"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "craf"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "cra"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "cr"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                default: // Invalid after "c"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        // Check for 'deca', 'disp', and 'duplus'
        case 'd': // Handles words starting with 'd'
            switch (token[1]) {
                case 'e': // Check for "de"
                    switch (token[2]) {
                        case 'c': // Check for "dec"
                            switch (token[3]) {
                                case 'a': // Check for "deca"
                                    if (token[4] == '\0') { // End of string confirms "deca"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "deca", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "deca"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "dec"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "de"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                case 'i': // Check for "di"
                    switch (token[2]) {
                        case 's': // Check for "dis"
                            switch (token[3]) {
                                case 'p': // Check for "disp"
                                    if (token[4] == '\0') { // End of string confirms "disp"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "disp", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "disp"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "dis"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "di"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                case 'u': // Check for "du"
                    switch (token[2]) {
                        case 'p': // Check for "dup"
                            switch (token[3]) {
                                case 'l': // Check for "dupl"
                                    switch (token[4]) {
                                        case 'u': // Check for "duplu"
                                            if (token[5] == 's' && token[6] == '\0') { // End of string confirms "duplus"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "duplus", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "duplus"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "dupl"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "dup"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;

                        case 'm': // Check for "dum"
                            if (token[3] == '\0') { // End of string confirms "dum"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "KEYWORD", "dum", lineNumber);
                                appendToBuffer(buffer, temp);
                            } else { // Extra characters after "dum"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "INVALID", token, lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;

                        default: // Invalid after "du"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                default: // Invalid after "d"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;
        
        // Check for 'endo' and 'exec'
        case 'e': // Handles words starting with 'e'
            switch (token[1]) {
                case 'n': // Check for "en"
                    switch (token[2]) {
                        case 'd': // Check for "end"
                            switch (token[3]) {
                                case 'o': // Check for "endo"
                                    if (token[4] == '\0') { // End of string confirms "endo"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "endo", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "endo"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "end"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "en"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                case 'x': // Check for "ex"
                    switch (token[2]) {
                        case 'e': // Check for "exe"
                            switch (token[3]) {
                                case 'c': // Check for "exec"
                                    if (token[4] == '\0') { // End of string confirms "exec"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "exec", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "exec"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "exe"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "ex"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;

                default: // Invalid after "e"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;
        
        
        case 'f': // Handles "f"
            switch (token[1]) {
                case 'a': // Check for "fa"
                    switch (token[2]) {
                        case 'l': // Check for "fal"
                            switch (token[3]) {
                                case 's': // Check for "fals"
                                    switch (token[4]) {
                                        case 'e': // Check for "false"
                                            if (token[5] == '\0') { // End of string confirms "false"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "RESERVED_WORD", "false", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "false"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "fals"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "fal"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "fa"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'e': // Check for "fe"
                    switch (token[2]) {
                        case 't': // Check for "fet"
                            switch (token[3]) {
                                case 'c': // Check for "fetc"
                                    switch (token[4]) {
                                        case 'h': // Check for "fetch"
                                            if (token[5] == '\0') { // End of string confirms "fetch"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "fetch", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "fetch"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "fetc"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "fet"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "fe"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'l': // Check for "fl"
                    switch (token[2]) {
                        case 'u': // Check for "flu"
                            switch (token[3]) {
                                case 'x': // Check for "flux"
                                    if (token[4] == '\0') { // End of string confirms "flux"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "KEYWORD", "flux", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "flux"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "flu"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "fl"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'n': // Check for "fn"
                    switch (token[2]) {
                        case '_': // Check for "fn_"
                            if (token[3] == '\0') { // End of string confirms "fn_"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "INVALID", token, lineNumber);
                                appendToBuffer(buffer, temp);
                            } else if (islower(token[3])) {
                                size_t tokenLength = strlen(token);  // Calculate length once
                                int valid = 1;

                                // Check all characters after the first one
                                for (int i = 4; i < tokenLength; i++) {
                                    if (!isalnum(token[i]) && token[i] != '_') {
                                        valid = 0;
                                        break;  // Return early if invalid
                                    }
                                }
                                if (valid) {
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "FUNCTION", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                } else {
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                }
                                break;
                            }
                            break;
                        default: // Invalid after "fn"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'o': // Check for "fo"
                    switch (token[2]) {
                        case 'r': // Check for "for"
                            switch (token[3]) {
                                case '\0': // End of string confirms "for"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "NOISE_WORD", "for", lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                                case 'm': // Check for "form"
                                    switch (token[4]) {
                                        case 'o': // Check for "formo"
                                            if (token[5] == '\0') { // End of string confirms "formo"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "formo", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "formo"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "form"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "for"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "fo"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'r': // Check for "fr"
                    switch (token[2]) {
                        case 'o': // Check for "fro"
                            switch (token[3]) {
                                case 'm': // Check for "from"
                                    if (token[4] == '\0') { // End of string confirms "from"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "NOISE_WORD", "from", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "from"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "fro"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "fr"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "f"  
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;
            
        case 'g': // Handles "g"
            switch (token[1]) {
                case 'o': // Check for "go"
                    switch (token[2]) {
                        case 't': // Check for "got"
                            switch (token[3]) {
                                case 'o': // Check for "goto"
                                    if (token[4] == '\0') { // End of string confirms "goto"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "RESERVED_WORD", "goto", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "goto"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "got"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "go"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "g"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 'n': // Handles "n"
            switch (token[1]) {
                case 'u': // Check for "nu"
                    switch (token[2]) {
                        case 'l': // Check for "nul"
                            switch (token[3]) {
                                case 'l': // Check for "null"
                                    if (token[4] == '\0') { // End of string confirms "null"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "RESERVED_WORD", "null", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "null"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "nul"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                        case 'm': // Check for "num"
                            switch (token[3]) {
                                case 'b': // Check for "numb"
                                    switch (token[4]) {
                                        case 'r': // Check for "numbr"
                                            switch (token[5]) {
                                                case 'a': // Check for "numbra"
                                                    if (token[6] == '\0') { // End of string confirms "numbra"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "KEYWORD", "numbra", lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    } else { // Extra characters after "numbra"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "INVALID", token, lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    }
                                                    break;
                                                default: // Invalid after "numbr"
                                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                            "INVALID", token, lineNumber);
                                                    appendToBuffer(buffer, temp);
                                                    break;
                                            }
                                            break;
                                        default: // Invalid after "numb"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "num"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    }
                                    break;
                                default: // Invalid after "nu"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "n"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;     
        case 'o': // Handles "o"
            switch (token[1]) {
                case 'p': // Check for "op"
                    switch (token[2]) {
                        case 't': // Check for "opt"
                            if (token[3] == '\0') { // End of string confirms "opt"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "KEYWORD", "opt", lineNumber);
                                appendToBuffer(buffer, temp);
                            } else { // Extra characters after "opt"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "INVALID", token, lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                        default: // Invalid after "op"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "o"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 'p': // Handles "p"
            switch (token[1]) {
                case 'r': // Check for "pr"
                    switch (token[2]) {
                        case 'o': // Check for "pro"
                            if (token[3] == '\0') { // End of string confirms "pro"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "KEYWORD", "pro", lineNumber);
                                appendToBuffer(buffer, temp);
                            } else { // Extra characters after "pro"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "INVALID", token, lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                        default: // Invalid after "pr"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "p"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 'q': // Handles "q"
            switch (token[1]) {
                case 'u': // Check for "qu"
                    switch (token[2]) {
                        case 'a': // Check for "qua"
                            switch (token[3]) {
                                case 'n': // Check for "quan"
                                    switch (token[4]) {
                                        case 'd': // Check for "quand"
                                            switch (token[5]) {
                                                case 'o': // Check for "quando"
                                                    if (token[6] == '\0') { // End of string confirms "quando"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "KEYWORD", "quando", lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    } else { // Extra characters after "quando"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "INVALID", token, lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    }
                                                    break;
                                                default: // Invalid after "quand"
                                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                            "INVALID", token, lineNumber);
                                                    appendToBuffer(buffer, temp);
                                                    break;
                                            }
                                            break;
                                        default: // Invalid after "quan"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "qua"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "qu"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "q"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 's': //Check for 's'
            switch (token[1]) {
                case 'e': // Check for "se"
                    switch (token[2]) {
                        case 't': // Check for "set"
                            if (token[3] == '\0') { // End of string confirms "set"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "KEYWORD", "set", lineNumber);
                                appendToBuffer(buffer, temp);
                            } else { // Extra characters after "set"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "INVALID", token, lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                        default: // Invalid after "se"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 'i': // Check for "si"
                    switch (token[2]) {
                        case 'g': // Check for "sig"
                            switch (token[3]) {
                                case 'n': // Check for "sign"
                                    switch (token[4]) {
                                        case 'a': // Check for "signa"
                                            if (token[5] == '\0') { // End of string confirms "signa"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "signa", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "signa"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "sign"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "sig"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "si"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                case 't': // Check for "st"
                    switch (token[2]) {
                        case 'a': // Check for "sta"
                            switch (token[3]) {
                                case 'r': // Check for "star"
                                    switch (token[4]) {
                                        case 't': // Check for "start"
                                            switch (token[5]) {
                                                case 'o': // Check for "starto"
                                                    if (token[6] == '\0') { // End of string confirms "starto"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "KEYWORD", "starto", lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    } else { // Extra characters after "starto"
                                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                                "INVALID", token, lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    }
                                                    break;
                                                default: // Invalid after "start"
                                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                            "INVALID", token, lineNumber);
                                                    appendToBuffer(buffer, temp);
                                                    break;
                                            }
                                            break;
                                        default: // Invalid after "star"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "sta"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "st"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "s"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 't': // Check for 't'
            switch (token[1]) {
                case 'o': // Check for "to"
                    if (token[2] == '\0') { // End of string confirms "to"
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", 
                                "NOISE_WORD", "to", lineNumber);
                        appendToBuffer(buffer, temp);
                    }
                case 'r': // Check for "tr"
                    switch (token[2]) {
                        case 'u': // Check for "tru"
                            switch (token[3]) {
                                case 'e': // Check for "true"
                                    if (token[4] == '\0') { // End of string confirms "true"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "RESERVED_WORD", "true", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    } else { // Extra characters after "true"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "INVALID", token, lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                                default: // Invalid after "tru"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "tr"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "t"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 'u': // Check for 'u'
            switch (token[1]) {
                case 'n': // Check for "un"
                    switch (token[2]) {
                        case 's': // Check for "uns"
                            switch (token[3]) {
                                case 'e': // Check for "unse"
                                    switch (token[4]) {
                                        case 't': // Check for "unset"
                                            if (token[5] == '\0') { // End of string confirms "unset"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "unset", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "unset"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "uns"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "uns"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                        case 't': // Check for "unt"
                            switch (token[3]) {
                                case 'i': // Check for "unti"
                                    switch (token[4]) {
                                        case 'l': // Check for "until"
                                            if (token[5] == '\0') { // End of string confirms "until"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "NOISE_WORD", "until", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "until"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "unti"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "unt"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "un"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "u"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;

        case 'y': // Check for 'y'
            switch (token[1]) {
                case 'i': // Check for "yi"
                    switch (token[2]) {
                        case 'e': // Check for "yie"
                            switch (token[3]) {
                                case 'l': // Check for "yiel"
                                    switch (token[4]) {
                                        case 'd': // Check for "yield"
                                            if (token[5] == '\0') { // End of string confirms "yield"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "KEYWORD", "yield", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            } else { // Extra characters after "yield"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "INVALID", token, lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                        default: // Invalid after "yiel"
                                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                    "INVALID", token, lineNumber);
                                            appendToBuffer(buffer, temp);
                                            break;
                                    }
                                    break;
                                default: // Invalid after "yie"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "INVALID", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                            }
                            break;
                        default: // Invalid after "yi"
                            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                    "INVALID", token, lineNumber);
                            appendToBuffer(buffer, temp);
                            break;
                    }
                    break;
                default: // Invalid after "y"
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                    break;
            }
            break;
        case '0': // Check for "0"
            if (token[1] == '\0') { // Single "0" is valid
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                        "INTEGER_LITERAL", "0", lineNumber);
                appendToBuffer(buffer, temp);
            } else if (isdigit(token[1])) { // Check for leading zero with additional digits
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                        "INVALID", token, lineNumber);
                appendToBuffer(buffer, temp);
            } else if (token[1] == '.') { // Check for floats starting with "0."
                size_t tokenLength = strlen(token);
                int valid = 1;

                for (int i = 2; i < tokenLength; i++) {
                    if (!isdigit(token[i])) {
                        valid = 0;
                        break;
                    }
                }

                if (valid) {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "FLOAT_LITERAL", token, lineNumber);
                } else {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                }
                appendToBuffer(buffer, temp);

            } else { // Invalid token
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                        "INVALID", token, lineNumber);
                appendToBuffer(buffer, temp);
            }
            break;

        case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': // Check for "1-9"
            if (token[1] == '\0') { // Single-digit integers
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                        "INTEGER_LITERAL", token, lineNumber);
                appendToBuffer(buffer, temp);
            } else {
                size_t tokenLength = strlen(token);
                int valid = 1;
                int hasDot = 0;

                for (int i = 0; i < tokenLength; i++) {
                    if (token[i] == '.') {
                        if (hasDot) { // More than one dot is invalid
                            valid = 0;
                            break;
                        }
                        hasDot = 1; // Mark that we found a dot
                    } else if (!isdigit(token[i])) {
                        valid = 0;
                        break;
                    }
                }

                if (valid) {
                    if (hasDot) { // It's a valid float literal
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                "FLOAT_LITERAL", token, lineNumber);
                    } else { // It's an integer literal
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                "INTEGER_LITERAL", token, lineNumber);
                    }
                } else {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                }
                appendToBuffer(buffer, temp);
            }
            break;
        case '"': // Handle string literals and format specifiers
            if (token[1] == '%') {
                if (token[2] == 'd' || token[2] == 'f' || token[2] == 'c' || token[2] == 's' && token[3] == '"' && token[4] == '\0') {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "FORMAT_SPECIFIER", token, lineNumber);
                    appendToBuffer(buffer, temp);
                } else {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                }
            } else {
                size_t tokenLength = strlen(token);
                if (token[tokenLength - 1] == '"') {
                    char valueWithoutQuotes[tokenLength - 1];
                    int index = 0;

                    for (int i = 1; i < tokenLength - 1; i++) {
                        valueWithoutQuotes[index++] = token[i];
                    }
                    valueWithoutQuotes[index] = '\0';

                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "STRING_LITERAL", token, lineNumber);
                    appendToBuffer(buffer, temp);
                } else {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                            "INVALID", token, lineNumber);
                    appendToBuffer(buffer, temp);
                }
            }
            break;



        default:
            if (token[0] == '\'' && token[strlen(token) - 1] == '\'') { // Character Literal
                // Extract the character between the single quotes
                char characterValue = token[1];  // Token is like 'a', so the character is at index 1

                snprintf(temp, sizeof(temp), "| %-25s | %-25c | %-10d |\n", "CHAR_LITERAL", characterValue, lineNumber);
                appendToBuffer(buffer, temp);
            } else if (token[0] == '@' && islower(token[1])) { // Variable
                size_t tokenLength = strlen(token);  // Calculate length once
                int valid = 1;

                // Check all characters after the first one
                for (int i = 2; i < tokenLength; i++) {
                    if (!isalnum(token[i]) && token[i] != '_') {
                        valid = 0;
                        break;  // Return early if invalid
                    }
                }

                // Classify and handle tokens
                if (valid) {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "VARIABLE", token, lineNumber);
                } else {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "INVALID", token, lineNumber);
                }
                appendToBuffer(buffer, temp);
            } else {
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "INVALID", token, lineNumber);
                appendToBuffer(buffer, temp);
            }
            break;
    }
}


void processLine(const char *line, int lineNumber, DynamicBuffer *buffer) {
    char token[256];
    int tokenIndex = 0;
    int insideStringLiteral = 0;  // To track if we're inside a string literal

    for (const char *ptr = line; *ptr; ptr++) {
        // Handle "//" line terminator delimiter
        if (*ptr == '/' && *(ptr + 1) == '/') {
            char delimiter[] = "//";
            classifyAndAppend(delimiter, lineNumber, buffer);
            break; // Stop processing the rest of the line
        } 

        // If inside a string literal, treat everything as part of the token
        if (insideStringLiteral) {
            token[tokenIndex++] = *ptr;  // Add the character to the token
            if (*ptr == '"') {  // Check for closing quote
                insideStringLiteral = 0;  // End of string literal
                token[tokenIndex] = '\0';  // Null-terminate the string
                classifyAndAppend(token, lineNumber, buffer);  // Process the token
                tokenIndex = 0;  // Reset tokenIndex for the next token
            }
            continue;  // Skip the rest of the loop and continue reading inside the string literal
        }

        switch (*ptr) {
            // If we encounter a quote, start a string literal
            case '"':
                insideStringLiteral = 1;
                token[tokenIndex++] = *ptr;  // Include the opening quote in the token
                break;

            // Single-character operators and delimiters
            case '+': case '-': case '*': case '/': case '^':
            case '=': case '!': case '>': case '<':
            case '(': case ')': case '{': case '}': case '[': case ']': case ',':
                if (tokenIndex > 0) {
                    token[tokenIndex] = '\0';
                    classifyAndAppend(token, lineNumber, buffer);
                    tokenIndex = 0;
                }

                // Handle two-character operators
                else if ((*ptr == '=' || *ptr == '!' || *ptr == '>' || *ptr == '<') && *(ptr + 1) == '=' || *ptr == '/' && *(ptr + 1) == '<') {
                    char twoCharToken[3] = {*ptr, *(ptr + 1), '\0'};
                    classifyAndAppend(twoCharToken, lineNumber, buffer);
                    ptr++; // Skip next character
                } else {
                    char singleCharToken[2] = {*ptr, '\0'};
                    classifyAndAppend(singleCharToken, lineNumber, buffer);
                }
                break;

            // Add characters to token
            default:
                if (!isspace(*ptr)) {
                    token[tokenIndex++] = *ptr;  // Add non-space characters to the token
                } else if (tokenIndex > 0) {
                    token[tokenIndex] = '\0';
                    classifyAndAppend(token, lineNumber, buffer);
                    tokenIndex = 0;
                }
                break;
        }
    }

    // If there's any remaining token, append it
    if (tokenIndex > 0) {
        token[tokenIndex] = '\0';
        classifyAndAppend(token, lineNumber, buffer);
    }
}

int main() {
    const char *sourceFileName = "output_stmt.leon";

    //Check the last five characters of the file name as .leon
    if (strlen(sourceFileName) < 5 || strcmp(sourceFileName + strlen(sourceFileName) - 5, ".leon") != 0) {
        fprintf(stderr, "The file must have a '.leon' extension.\n");
        return EXIT_FAILURE;
    }

    //Open file if valid
    FILE *sourceFile = fopen(sourceFileName, "r");
    if (!sourceFile) {
        fprintf(stderr, "Unable to open file: %s\n", sourceFileName);
        return EXIT_FAILURE;
    }

    char line[512];
    int lineNumber = 0;

    DynamicBuffer buffer;
    initBuffer(&buffer, INITIAL_BUFFER_SIZE);

    appendToBuffer(&buffer, "+---------------------------+---------------------------+------------+\n");
    appendToBuffer(&buffer, "| Token Type                | Lexeme                    | Line       |\n");
    appendToBuffer(&buffer, "+---------------------------+---------------------------+------------+\n");

    while (fgets(line, sizeof(line), sourceFile)) {
        lineNumber++;
        processLine(line, lineNumber, &buffer);
    }

    appendToBuffer(&buffer, "+---------------------------+---------------------------+------------+\n");

    fclose(sourceFile);

    FILE *outputFile = fopen("symbolTable1.leon", "w");
    if (!outputFile) {
        fprintf(stderr, "Unable to create output file.\n");
        freeBuffer(&buffer);
        return EXIT_FAILURE;
    }

    fputs(buffer.data, outputFile);
    fclose(outputFile);

    freeBuffer(&buffer);

    printf("Tokenization complete. Results saved to 'symbolTable1.leon'.\n");

    return EXIT_SUCCESS;
}
