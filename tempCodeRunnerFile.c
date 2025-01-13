            // Remove the surrounding quotes
                char valueWithoutQuotes[strlen(token) - 1];
                int index = 0;

                for (int i = 1; i < strlen(token) - 1; i++) { // Start after the first quote and end before the last quote
                    valueWithoutQuotes[index++] = token[i]; // Copy all characters, including '%'
                }
                valueWithoutQuotes[index] = '\0';  // Null-terminate the string

                snprintf(temp, sizeof(temp), "| %-25s | %-25s | %-10d |\n", "STRING_LITERAL", valueWithoutQuotes, lineNumber);
                appendToBuffer(buffer, temp);