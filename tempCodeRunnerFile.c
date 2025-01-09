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