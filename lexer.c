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
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Arithmetic Operator", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '=': // Assignment Operator or Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", "==", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Assignment Operator", "=", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '!': // Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", "!=", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '>': // Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", ">=", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", ">", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '<': // Relational Operator
            if (token[1] == '=')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", "<=", lineNumber);
            else
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Relational Operator", "<", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '&': // Logical Operator
            if (token[1] == '&')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Logical Operator", "&&", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '|': // Logical Operator
            if (token[1] == '|')
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Logical Operator", "||", lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '/': // Arithmetic Operator or Delimiter
            if (token[1] == '/') {
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Delimiter", "//", lineNumber);
                appendToBuffer(buffer, temp);
            } else {
                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Arithmetic Operator", "/", lineNumber);
                appendToBuffer(buffer, temp);
            }
            break;
        case '(': case ')': case '{': case '}': case '[': case ']': case ',': // Delimiters
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Delimiter", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;
        case '$': // Comment
            snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Comment", token, lineNumber);
            appendToBuffer(buffer, temp);
            break;

        // Check for 'and' and 'alt' as noise words
        case 'a': // Handles words starting with 'a'
            switch (token[1]) {
                case 'n': // Check for "and"
                    switch (token[2]) {
                        case 'd':
                            if (token[3] == '\0') { // End of string
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "Noise Word", "and", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                    }
                    break;

                case 'l': // Check for "alt"
                    switch (token[2]) {
                        case 't':
                            if (token[3] == '\0') { // End of string
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "Keyword", "alt", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                    }
                    break;
            }
            break;

        //Check for 'binar' and 'bind' as keywords
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
                                                        "Keyword", "binar", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;

                                case 'd': // Check for "bind"
                                    if (token[4] == '\0') { // End of string
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "Keyword", "bind", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;

        //Check for 'const' and 'craft' as reserved word and keyword respectively
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
                                                        "Reserved Word", "const", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
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
                                                        "Keyword", "craft", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;
        case 'd': // Handles "d"
            switch (token[1]) {
                case 'e': // Check for "de"
                    switch (token[2]) {
                        case 'c': // Check for "dec"
                            switch (token[3]) {
                                case 'a': // Check for "deca"
                                    if (token[4] == '\0') { // End of string confirms "deca"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "Keyword", "deca", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
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
                                                "Keyword", "disp", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                case 'u': // Check for "du"
                    switch (token[2]) {
                        case 'm': // Check for "dum"
                            if (token[3] == '\0') { // End of string confirms "dum"
                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                        "Keyword", "dum", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;

                        case 'l': // Check for "dupl"
                            switch (token[3]) {
                                case 'o': // Check for "duplo"
                                    switch (token[4]) {
                                        case 's': // Check for "duplos"
                                            if (token[5] == '\0') { // End of string confirms "duplos"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "Keyword", "duplos", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;

        
        case 'e': // Handles "e"
            switch (token[1]) {
                case 'n': // Check for "en"
                    switch (token[2]) {
                        case 'd': // Check for "end"
                            switch (token[3]) {
                                case 'o': // Check for "endo"
                                    if (token[4] == '\0') { // End of string confirms "endo"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "Keyword", "endo", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
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
                                                "Keyword", "exec", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
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
                                                        "Reserved Word", "false", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
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
                                                        "Keyword", "fetch", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
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
                                                "Keyword", "flux", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                case 'n': // Check for "fn"
                    switch (token[2]) {
                        case '_': // Check for "fn_"
                            if (islower(token[3])) {
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
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Function", token, lineNumber);
                                    appendToBuffer(buffer, temp);
                                }
                            }
                            break;
                    }
                    break;
                case 'o': // Check for "fo"
                    switch (token[2]) {
                        case 'r': // Check for "for"
                            switch (token[3]) {
                                case '\0': // End of string confirms "for"
                                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                            "Noise Word", "for", lineNumber);
                                    appendToBuffer(buffer, temp);
                                    break;
                                case 'm': // Check for "form"
                                    switch (token[4]) {
                                        case 'o': // Check for "formo"
                                            if (token[5] == '\0') { // End of string confirms "formo"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "Keyword", "formo", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
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
                                                "Noise Word", "from", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
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
                                                "Reserved Word", "goto", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
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
                                                "Reserved Word", "null", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                        case 'm': // Check for "num"
                            switch (token[3]) {
                                case 'b': // Check for "numb"
                                    if (token[4] == '\0') { // End of string confirms "numb"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "Keyword", "numb", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
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
                                        "Keyword", "opt", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                    }
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
                                        "Keyword", "pro", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
                            break;
                    }
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
                                                                "Keyword", "quando", lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    }
                                                    break;
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
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
                                        "Keyword", "set", lineNumber);
                                appendToBuffer(buffer, temp);
                            }
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
                                                        "Keyword", "signa", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
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
                                                                "Keyword", "starto", lineNumber);
                                                        appendToBuffer(buffer, temp);
                                                    }
                                                    break;
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;

        case 't': // Check for 't'
            switch (token[1]) {
                case 'o': // Check for "to"
                    if (token[2] == '\0') { // End of string confirms "to"
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                "Noise Word", "to", lineNumber);
                        appendToBuffer(buffer, temp);
                    }
                case 'r': // Check for "tr"
                    switch (token[2]) {
                        case 'u': // Check for "tru"
                            switch (token[3]) {
                                case 'e': // Check for "true"
                                    if (token[4] == '\0') { // End of string confirms "true"
                                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                "Reserved Word", "true", lineNumber);
                                        appendToBuffer(buffer, temp);
                                    }
                                    break;
                            }
                            break;
                    }
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
                                                        "Keyword", "unset", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
                        case 't': // Check for "unt"
                            switch (token[3]) {
                                case 'i': // Check for "unti"
                                    switch (token[4]) {
                                        case 'l': // Check for "until"
                                            if (token[5] == '\0') { // End of string confirms "until"
                                                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n",
                                                        "Noise Word", "until", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
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
                                                        "Keyword", "yield", lineNumber);
                                                appendToBuffer(buffer, temp);
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;

        default:
            if (token[0] == '@' && (islower(token[1]) || token[1] == '_')) { // Variable Identifier
                size_t tokenLength = strlen(token);  // Calculate length once
                int valid = 1;

                // Check all remaining characters after the first one
                for (int i = 1; i < tokenLength; i++) {
                    if (!isalnum(token[i]) && token[i] != '_') {
                        valid = 0;
                        break;  // Return early if invalid
                    }
                }
                if (valid) {
                    snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Variable", token, lineNumber);
                    appendToBuffer(buffer, temp);
                }
            } else if (isdigit(token[0])) { // Constant Value (Integer or Float)
                size_t tokenLength = strlen(token);  // Calculate length once
                int valid = 1;
                int hasDot = 0;  // Flag to track if there's a dot

                // Check all characters after the first one
                for (int i = 1; i < tokenLength; i++) {
                    if (token[i] == '.') {
                        if (hasDot) {  // More than one dot is invalid
                            valid = 0;
                            break;
                        }
                        hasDot = 1;  // Mark that we found a dot
                    } else if (!isdigit(token[i])) {
                        valid = 0;
                        break;  // Return early if invalid
                    }
                }

                // Classify token based on the presence of a dot
                if (valid) {
                    if (hasDot) {
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Constant Value (Float)", token, lineNumber);
                    } else {
                        snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "Constant Value (Integer)", token, lineNumber);
                    }
                    appendToBuffer(buffer, temp);
                }
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

    for (const char *ptr = line; *ptr; ptr++) {
        // Handle "//" line terminator delimiter
        if (*ptr == '/' && *(ptr + 1) == '/') {
            char commentDelimiter[] = "//";
            classifyAndAppend(commentDelimiter, lineNumber, buffer);
            break; // Stop processing the rest of the line
        }

        switch (*ptr) {
            // Single-character operators and delimiters
            case '+': case '-': case '*': case '/': case '%': case '^':
            case '=': case '!': case '>': case '<':
            case '(': case ')': case '{': case '}': case '[': case ']': case ',':
                if (tokenIndex > 0) {
                    token[tokenIndex] = '\0';
                    classifyAndAppend(token, lineNumber, buffer);
                    tokenIndex = 0;
                }

                // Handle two-character operators
                if ((*ptr == '=' || *ptr == '!' || *ptr == '>' || *ptr == '<') && *(ptr + 1) == '=') {
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
                    token[tokenIndex++] = *ptr;
                } else if (tokenIndex > 0) {
                    token[tokenIndex] = '\0';
                    classifyAndAppend(token, lineNumber, buffer);
                    tokenIndex = 0;
                }
                break;
        }
    }

    if (tokenIndex > 0) {
        token[tokenIndex] = '\0';
        classifyAndAppend(token, lineNumber, buffer);
    }
}

int main() {
    const char *sourceFileName = "sample.leon";
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

    printf("Tokenization complete. Results saved to 'symbolTable.leon'.\n");

    return EXIT_SUCCESS;
}
