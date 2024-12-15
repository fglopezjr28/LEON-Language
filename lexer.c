#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 2000

void appendToBuffer(char *buffer, int *bufferIndex, const char *tokenType, const char *tokenValue, int lineNumber) {
    *bufferIndex += sprintf(buffer + *bufferIndex, "| %-25s | %-25s | %-10d |\n", tokenType, tokenValue, lineNumber);
}

void isVariableIdentifier(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    // Check if the token starts with '@' and the second character is lowercase or '_'
    if (token[0] == '@' && (islower(token[1]) || token[1] == '_')) { 
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
            appendToBuffer(buffer, bufferIndex, "Variable", token, lineNumber);
        }
    }
}

void isFunctionIdentifier(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    if (isalpha(token[0])) {  // Check if the first character is alphabetic
        size_t tokenLength = strlen(token);  // Calculate length once
        int valid = 1;

        // Check all characters after the first one
        for (int i = 1; i < tokenLength; i++) {
            if (!isalnum(token[i]) && token[i] != '_') {
                valid = 0;
                break;  // Return early if invalid
            }
        }

        if (valid) {
            appendToBuffer(buffer, bufferIndex, "Function", token, lineNumber);
        }
    }
}

void isDelimiter(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    switch (token[0]) {
        case '/':
            if (token[1] == '/') {
                appendToBuffer(buffer, bufferIndex, "Delimiter", "//", lineNumber);
            } else if (token[1] == '<') {
                appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "/<", lineNumber);
            } else {
                appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "/", lineNumber);
            }
            break;
        case ',':
            appendToBuffer(buffer, bufferIndex, "Delimiter", ",", lineNumber);
            break;
        case '(':
            appendToBuffer(buffer, bufferIndex, "Delimiter", "(", lineNumber);
            break;
        case ')':
            appendToBuffer(buffer, bufferIndex, "Delimiter", ")", lineNumber);
            break;
        case '{':
            appendToBuffer(buffer, bufferIndex, "Delimiter", "{", lineNumber);
            break;
        case '}':
            appendToBuffer(buffer, bufferIndex, "Delimiter", "}", lineNumber);
            break;
        case '[':
            appendToBuffer(buffer, bufferIndex, "Delimiter", "[", lineNumber);
            break;
        case ']':
            appendToBuffer(buffer, bufferIndex, "Delimiter", "]", lineNumber);
            break;
        default:
            break;
    }
}

void isOperator(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    switch (token[0]) {
        case '+':
            appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "+", lineNumber);
            break;
        case '-':
            appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "-", lineNumber);
            break;
        case '*':
            appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "*", lineNumber);
            break;
        case '/':
            if (token[1] == '/') {  // Handle "//" as a delimiter
                appendToBuffer(buffer, bufferIndex, "Delimiter", "//", lineNumber);
            } else {
                appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "/", lineNumber);
            }
            break;
        case '%':
            appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "%", lineNumber);
            break;
        case '^':
            appendToBuffer(buffer, bufferIndex, "Arithmetic Operator", "^", lineNumber);
            break;
        case '=':
            if (token[1] == '=') {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", "==", lineNumber);
            } else {
                appendToBuffer(buffer, bufferIndex, "Assignment Operator", "=", lineNumber);
            }
            break;
        case '!':
            if (token[1] == '=') {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", "!=", lineNumber);
            }
            break;
        case '>':
            if (token[1] == '=') {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", ">=", lineNumber);
            } else {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", ">", lineNumber);
            }
            break;
        case '<':
            if (token[1] == '=') {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", "<=", lineNumber);
            } else {
                appendToBuffer(buffer, bufferIndex, "Relational Operator", "<", lineNumber);
            }
            break;
        default:
            break;
    }
}

int isConstantValue(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    // Check for integer
    if (isdigit(token[0])) {
        int valid = 1;
        for (int i = 1; i < strlen(token); i++) {
            if (!isdigit(token[i])) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            appendToBuffer(buffer, bufferIndex, "Constant (Integer)", token, lineNumber);
            return 1;
        }
    }

    // Check for float
    int dotCount = 0;
    if (isdigit(token[0]) || token[0] == '.') {
        int valid = 1;
        for (int i = 0; i < strlen(token); i++) {
            if (token[i] == '.') {
                dotCount++;
                if (dotCount > 1) {
                    valid = 0;
                    break;
                }
            } else if (!isdigit(token[i])) {
                valid = 0;
                break;
            }
        }
        if (valid && dotCount == 1) {
            appendToBuffer(buffer, bufferIndex, "Constant (Float)", token, lineNumber);
            return 1;
        }
    }

    switch (token[0]) {
        case '\'':
            if (isalpha(token[1]) && token[2] == '\'') {
                appendToBuffer(buffer, bufferIndex, "Constant (Character)", token, lineNumber);
                return 1;
            } else {
                appendToBuffer(buffer, bufferIndex, "Invalid Character", token, lineNumber);
                return 1;
            }
        case '\"':
            {
                int valid = 1;
                for (int i = 1; i < strlen(token) - 1; i++) {
                    if (!isalpha(token[i])) {
                        valid = 0;
                        break;
                    }
                }
                if (valid && token[strlen(token) - 1] == '\"') {
                    appendToBuffer(buffer, bufferIndex, "Constant (String)", token, lineNumber);
                    return 1;
                } else {
                    appendToBuffer(buffer, bufferIndex, "Invalid String", token, lineNumber);
                    return 1;
                }
            }
            break;
        default:
            break;
    }
    return 0;
}

void isComment(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    size_t len = strlen(token);
    if (len >= 2 && token[0] == '$' && token[len - 1] == '$') {
        appendToBuffer(buffer, bufferIndex, "Comment", token, lineNumber);
    }
}

int isKeyword(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    switch (token[0]) {
        case 'a':
            switch (token[1]) {
                case 'l':
                    switch (token[2]) {
                        case 't':
                            if (token[3] == '\0') {
                                appendToBuffer(buffer, bufferIndex, "Keyword", "alt", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;
            }
            break;

        case 'b':
            switch (token[1]) {
                case 'i':
                    switch (token[2]) {
                        case 'n':
                            switch (token[3]) {
                                case 'd':
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "bind", lineNumber);
                                        return 1;
                                    }
                                    break;
                                case 'a':
                                    switch (token[4]) {
                                        case 'r':
                                            if (token[5] == '\0') {
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "binar", lineNumber);
                                                return 1;
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

        case 'c':
            switch (token[1]) {
                case 'r':
                    switch (token[2]) {
                        case 'a':
                            switch (token[3]) {
                                case 'f':
                                    switch (token[4]) {
                                        case 't':
                                            if (token[5] == '\0') {
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "craft", lineNumber);
                                                return 1;
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

        case 'd':
            switch (token[1]) {
                case 'e':
                    switch (token[2]) {
                        case 'c':
                            switch (token[3]) {
                                case 'a':
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "deca", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                case 'i':
                    switch (token[2]) {
                        case 's':
                            switch (token[3]) {
                                case 'p':
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "disp", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                case 'u':
                    switch (token[2]) {
                        case 'm':
                            if (token[3] == '\0') {
                                appendToBuffer(buffer, bufferIndex, "Keyword", "dum", lineNumber);
                                return 1;
                            }
                            break;
                        case 'p':
                            switch (token[3]) {
                                case 'l':
                                    switch (token[4]) {
                                        case 'u':
                                            switch (token[5]) {
                                                case 's':
                                                    if (token[6] == '\0') {
                                                        appendToBuffer(buffer, bufferIndex, "Keyword", "duplus", lineNumber);
                                                        return 1;
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

        case 'e':
            switch (token[1]) {
                case 'n':
                    switch (token[2]) {
                        case 'd':
                            switch (token[3]) {
                                case 'o':
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "endo", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
                case 'x':
                    switch (token[2]) {
                        case 'e':
                            switch (token[3]) {
                                case 'c':
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "exec", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;

        case 'f':
            switch (token[1]) {
                case 'e': // Check for keywords starting with "fe"
                    switch (token[2]) {
                        case 't': // Check for "fet"
                            switch (token[3]) {
                                case 'c': // Check for "fetch"
                                    switch (token[4]) {
                                        case 'h':
                                            if (token[5] == '\0') { // Ensure no extra characters
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "fetch", lineNumber);
                                                return 1;
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;

                case 'l': // Check for keywords starting with "fl"
                    switch (token[2]) {
                        case 'u': // Check for "flu"
                            switch (token[3]) {
                                case 'x': // Check for "flux"
                                    if (token[4] == '\0') { // Ensure no extra characters
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "flux", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;

                case 'o': // Check for keywords starting with "fo"
                    switch (token[2]) {
                        case 'r': // Check for "for"
                            switch (token[3]) {
                                case 'm': // Check for "form"
                                    switch (token[4]) {
                                        case 'o': // Check for "formo"
                                            if (token[5] == '\0') { // Ensure no extra characters
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "formo", lineNumber);
                                                return 1;
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

        case 'n':
            switch (token[1]) {
                case 'u': // Check for keywords starting with "nu"
                    switch (token[2]) {
                        case 'm': // Check for "num"
                            switch (token[3]) {
                                case 'b': // Check for "numb"
                                    if (token[4] == '\0') { // Ensure no extra characters
                                        appendToBuffer(buffer, bufferIndex, "Keyword", "numb", lineNumber);
                                        return 1;
                                    }
                                    break; 
                            }
                            break;
                    }
                    break;
            }
            break;

        case 'o':
            switch (token[1]) {
                case 'p': // Check for keywords starting with "op"
                    switch (token[2]) {
                        case 't': // Check for "opt"
                            if (token[3] == '\0') { // Ensure no extra characters
                                appendToBuffer(buffer, bufferIndex, "Keyword", "opt", lineNumber);
                                return 1;
                            }
                            break; 
                    }
                    break; 
            }
            break;

        case 'p':
            switch (token[1]) {
                case 'r': // Check for keywords starting with "pr"
                    switch (token[2]) {
                        case 'o': // Check for "pro"
                            if (token[3] == '\0') { // Ensure no extra characters
                                appendToBuffer(buffer, bufferIndex, "Keyword", "pro", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;
            }
            break;

        case 'q':
            switch (token[1]) {
                case 'u': // Check for keywords starting with "qu"
                    switch (token[2]) {
                        case 'a': // Check for "qua"
                            switch (token[3]) {
                                case 'n': // Check for "quan"
                                    switch (token[4]) {
                                        case 'd': // Check for "quand"
                                            switch (token[5]) {
                                                case 'o': // Check for "quando"
                                                    if (token[6] == '\0') { // Ensure no extra characters
                                                        appendToBuffer(buffer, bufferIndex, "Keyword", "quando", lineNumber);
                                                        return 1;
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

        case 's':
            switch (token[1]) {
                case 'e': // Check for keywords starting with "se"
                    switch (token[2]) {
                        case 't': // Check for "set"
                            if (token[3] == '\0') { // Ensure no extra characters
                                appendToBuffer(buffer, bufferIndex, "Keyword", "set", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;

                case 'i': // Check for keywords starting with "si"
                    switch (token[2]) {
                        case 'g': // Check for "sig"
                            switch (token[3]) {
                                case 'n': // Check for "sign"
                                    switch (token[4]) {
                                        case 'a': // Check for "signa"
                                            if (token[5] == '\0') { // Ensure no extra characters
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "signa", lineNumber);
                                                return 1;
                                            }
                                            break;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;

                case 't': // Check for keywords starting with "st"
                    switch (token[2]) {
                        case 'a': // Check for "sta"
                            switch (token[3]) {
                                case 'r': // Check for "star"
                                    switch (token[4]) {
                                        case 't': // Check for "start"
                                            switch (token[5]) {
                                                case 'o': // Check for "starto"
                                                    if (token[6] == '\0') { // Ensure no extra characters
                                                        appendToBuffer(buffer, bufferIndex, "Keyword", "starto", lineNumber);
                                                        return 1;
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

        case 'u':
            switch (token[1]) {
                case 'n': // Check for keywords starting with "un"
                    switch (token[2]) {
                        case 's': // Check for "uns"
                            switch (token[3]) {
                                case 'e': // Check for "unse"
                                    switch (token[4]) {
                                        case 't': // Check for "unset"
                                            if (token[5] == '\0') { // Ensure no extra characters
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "unset", lineNumber);
                                                return 1;
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

        case 'y':
            switch (token[1]) {
                case 'i': // Check for keywords starting with "yi"
                    switch (token[2]) {
                        case 'e': // Check for "yie"
                            switch (token[3]) {
                                case 'l': // Check for "yiel"
                                    switch (token[4]) {
                                        case 'd': // Check for "yield"
                                            if (token[5] == '\0') { // Ensure no extra characters
                                                appendToBuffer(buffer, bufferIndex, "Keyword", "yield", lineNumber);
                                                return 1;
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

    return 0; // Not a recognized keyword
}

int isReservedWord(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    switch (token[0]) {
    case 'c':  // Check for "c"
        switch (token[1]) {
            case 'o':  // Check for "co"
                switch (token[2]) {
                    case 'n':  // Check for "con"
                        switch (token[3]) {
                            case 's':  // Check for "cons"
                                switch (token[4]) {
                                    case 't':  // Check for "const"
                                        if (token[5] == '\0') {  // Ensure no extra characters
                                            appendToBuffer(buffer, bufferIndex, "Reserved Word", "const", lineNumber);
                                            return 1;
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
    case 'g':  // Check for "g"
        switch (token[1]) {
            case 'o':  // Check for "go"
                switch (token[2]) {
                    case 't':  // Check for "got"
                        switch (token[3]) {
                            case 'o':  // Check for "goto"
                                if (token[4] == '\0') {  // Ensure no extra characters
                                    appendToBuffer(buffer, bufferIndex, "Reserved Word", "goto", lineNumber);
                                    return 1;
                                }
                                break;
                        }
                        break;
                }
                break;
        }
        break;
    case 'n':  // Check for "n"
        switch (token[1]) {
            case 'u':  // Check for "nu"
                switch (token[2]) {
                    case 'l':  // Check for "nul"
                        switch (token[3]) {
                            case 'l':  // Check for "null"
                                if (token[4] == '\0') {  // Ensure no extra characters
                                    appendToBuffer(buffer, bufferIndex, "Reserved Word", "null", lineNumber);
                                    return 1;
                                }
                                break;
                        }
                        break;
                }
                break;
        }
        break;
    case 't': // Check for "t"
            switch (token[1]) { 
                case 'r': // Check for "tr"
                    switch (token[2]) { 
                        case 'u': // Check for "tru"
                            switch (token[3]) { 
                                case 'e': // Check for "true"
                                    appendToBuffer(buffer, bufferIndex, "Reserved Word", "true", lineNumber);
                                    return 1;
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break;
        case 'f': // Check for "f"
            switch (token[1]) {
                case 'a': // Check for "fa"
                    switch (token[2]) {
                        case 'l': // Check for "fal"
                            switch (token[3]) {
                                case 's': // Check for "fals"
                                    switch (token[4]) {
                                        case 'e': // Check for "false"
                                            appendToBuffer(buffer, bufferIndex, "Reserved Word", "false", lineNumber);
                                            return 1;
                                            break;
                                        default:
                                            break;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
            break; 
        break;
}
}

int isNoiseWord(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    switch (token[0]) {
        case 't': // Check for "t"
            switch (token[1]) {
                case 'h': // Check for "th"
                    switch (token[2]) {
                        case 'e': // Check for "the"
                            if (token[3] == '\0') {
                                appendToBuffer(buffer, bufferIndex, "Noise Word", "the", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;
                case 'o': // Check for "to"
                    if (token[2] == '\0') {
                        appendToBuffer(buffer, bufferIndex, "Noise Word", "to", lineNumber);
                        return 1;
                    }
                    break;
            }
            break;
        case 'a': // Check for "a"
            switch (token[1]) {
                case 'n': // Check for "an"
                    switch (token[2]) {
                        case 'd': // Check for "and"
                            if (token[3] == '\0') {
                                appendToBuffer(buffer, bufferIndex, "Noise Word", "and", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;
            }
            break;
        case 'o': // Check for "o"
            switch (token[1]) {
                case 'f': // Check for "of"
                    if (token[2] == '\0') {
                        appendToBuffer(buffer, bufferIndex, "Noise Word", "of", lineNumber);
                        return 1;
                    }
                    break;
            }
            break;
        case 'f': // Check for "f"
            switch (token[1]) {
                case 'o': // Check for "fo"
                    switch (token[2]) {
                        case 'r': // Check for "for"
                            if (token[3] == '\0') {
                                appendToBuffer(buffer, bufferIndex, "Noise Word", "for", lineNumber);
                                return 1;
                            }
                            break;
                    }
                    break;
                case 'r': // Check for "fr"
                    switch (token[2]) {
                        case 'o': // Check for "fro"
                            switch (token[3]) {
                                case 'm': // Check for "from"
                                    if (token[4] == '\0') {
                                        appendToBuffer(buffer, bufferIndex, "Noise Word", "from", lineNumber);
                                        return 1;
                                    }
                                    break;
                            }
                            break;
                    }
                    break;
            }
            break;
        case 'u': // Check for "u"
            switch (token[1]) {
                case 'n': // Check for "un"
                    switch (token[2]) {
                        case 't': // Check for "unt"
                            switch (token[3]) {
                                case 'i': // Check for "unti"
                                    switch (token[4]) {
                                        case 'l': // Check for "until"
                                            if (token[5] == '\0') {
                                                appendToBuffer(buffer, bufferIndex, "Noise Word", "until", lineNumber);
                                                return 1;
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
    return 0; // Return false if no match is found
}

void trimWhitespace(char *str) {
    int start = 0, end = strlen(str) - 1;
    
    // Find the first non-whitespace character
    while (str[start] == ' ' || str[start] == '\t') start++;
    
    // Find the last non-whitespace character
    while (end >= start && (str[end] == ' ' || str[end] == '\t')) end--;
    
    // Shift characters to the left
    for (int i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    
    str[end - start + 1] = '\0';  // Null-terminate the string
}

void processToken(const char *token, int lineNumber, char *buffer, int *bufferIndex) {
    char tempBuffer[255];
    int j = 0;
    int inComment = 0;
    int tokenLength = (int)strlen(token);

    // Process each character in the token
    for (int i = 0; i < tokenLength; i++) {
        // Handle start of comment (using $)
        if (token[i] == '$' && (i == 0 || token[i - 1] == ' ')) {
            inComment = 1; // Start of comment
        }

        // Handle comment processing
        if (inComment) {
            tempBuffer[j++] = token[i];
            if (token[i] == '$' && (i == tokenLength - 1 || token[i + 1] == ' ')) {
                tempBuffer[j] = '\0'; // End of comment
                isComment(tempBuffer, lineNumber, buffer, bufferIndex);
                j = 0;
                inComment = 0;
            }
        }
        // Handle delimiters and operators
        else if (strchr("(),{}[]", token[i]) || strchr("+-*/%^!=><", token[i])) {
            if (j > 0) { // Process the token before the delimiter/operator
                tempBuffer[j] = '\0';
                trimWhitespace(tempBuffer);
                if (isNoiseWord(tempBuffer, lineNumber, buffer, bufferIndex)) return;
                if (isKeyword(tempBuffer, lineNumber, buffer, bufferIndex)) return;
                if (isReservedWord(tempBuffer, lineNumber, buffer, bufferIndex)) return;
                if (!isConstantValue(tempBuffer, lineNumber, buffer, bufferIndex)) {
                    isVariableIdentifier(tempBuffer, lineNumber, buffer, bufferIndex);
                    isFunctionIdentifier(tempBuffer, lineNumber, buffer, bufferIndex);
                    isOperator(tempBuffer, lineNumber, buffer, bufferIndex);
                }
                j = 0; // Reset tempBuffer
            }

            // Handle specific operators and delimiters
            if (token[i] == '/' && token[i + 1] == '/') {
                appendToBuffer(buffer, bufferIndex, "Delimiter", "//", lineNumber);
                break; // Skip the rest of the line
            } else {
                char delimiter[2] = {token[i], '\0'};
                isDelimiter(delimiter, lineNumber, buffer, bufferIndex);
            }
        }
        // Handle variable identifiers
        else if (token[i] == '@') {
            tempBuffer[j++] = token[i];
            while (i + 1 < tokenLength && (isalnum(token[i + 1]) || token[i + 1] == '_')) {
                tempBuffer[j++] = token[++i];
            }
            tempBuffer[j] = '\0';
            isVariableIdentifier(tempBuffer, lineNumber, buffer, bufferIndex);
            j = 0; // Reset tempBuffer
        }
        // Handle normal characters (add to tempBuffer)
        else {
            tempBuffer[j++] = token[i];
        }
    }

    // Process remaining characters in tempBuffer after loop
    if (j > 0) {
        tempBuffer[j] = '\0';
        trimWhitespace(tempBuffer);
        if (isNoiseWord(tempBuffer, lineNumber, buffer, bufferIndex)) return;
        if (isKeyword(tempBuffer, lineNumber, buffer, bufferIndex)) return;
        if (isReservedWord(tempBuffer, lineNumber, buffer, bufferIndex)) return;
        if (!isConstantValue(tempBuffer, lineNumber, buffer, bufferIndex)) {
            isVariableIdentifier(tempBuffer, lineNumber, buffer, bufferIndex);
            isFunctionIdentifier(tempBuffer, lineNumber, buffer, bufferIndex);
            isOperator(tempBuffer, lineNumber, buffer, bufferIndex);
        }
    }
}


int main() {
    const char *sourceFileName = "example.leon";
    FILE *sourceFile = fopen(sourceFileName, "r");
    char buffer[255];
    int lineNumber = 0;

    if (sourceFile == NULL) {
        printf("UNABLE TO OPEN FILE.\n");
        return 1;
    }

    // Pre-allocate a buffer for output to improve performance
    char outputBuffer[MAX_LENGTH * 10] = {0}; // Buffer for accumulating the results
    int bufferIndex = 0;

    bufferIndex += sprintf(outputBuffer + bufferIndex, "+---------------------------+---------------------------+------------+\n");
    bufferIndex += sprintf(outputBuffer + bufferIndex, "| Token Type                | Lexeme                    | Line       |\n");
    bufferIndex += sprintf(outputBuffer + bufferIndex, "+---------------------------+---------------------------+------------+\n");

    // Check if the filename ends with ".leon"
    if (strlen(sourceFileName) >= 5 && strcmp(&sourceFileName[strlen(sourceFileName) - 5], ".leon") == 0) {
        while (fgets(buffer, sizeof(buffer), sourceFile) != NULL) {
            lineNumber++;
            char *token = strtok(buffer, " \n");
            while (token != NULL) {
                processToken(token, lineNumber, outputBuffer, &bufferIndex);
                token = strtok(NULL, " \n");
            }
        }

        bufferIndex += sprintf(outputBuffer + bufferIndex, "+---------------------------+---------------------------+------------+\n");

        // Write the entire buffer to the output file at once
        FILE *symbolTable = fopen("symbolTable.leon", "w");
        fputs(outputBuffer, symbolTable);
        fclose(symbolTable);

    } else {
        printf("INVALID FILE.\n");
    }

    fclose(sourceFile);

    return 0;
}