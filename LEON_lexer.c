//LEON Lexical Analyzer
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 300

// Function to check for a delimiter
bool isDelimiter(char chr)
{
    return (chr == '+' || chr == '-' || chr == '*' || chr == '/' ||
            chr == ',' || chr == ';' || chr == '%' || chr == '>' || chr == '<' ||
            chr == '(' || chr == ')' || chr == '[' || chr == ']' || chr == '{' || chr == '}');
}

// Function to check for valid operators
bool isOperator(char chr)
{
    return (chr == '+' || chr == '-' || chr == '*' || chr == '/' || chr == '>' || 
            chr == '<' || chr == '=');
}

// Function to check if a string is a double-slash line terminator
bool isLineTerminator(char* str)
{
    return (str[0] == '/' && str[1] == '/');
}

// Function to check if the identifier starts with '@' and is valid
bool isValidIdentifier(char* str)
{
    if (str[0] != '@') {
        return false;  // Identifiers must start with '@'
    }
    // Check the rest of the characters are alphanumeric or underscores
    int i;
    for (i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i]) && str[i] != '_') {
            return false;
        }
    }
    return true;
}

// Function to check for a keyword
bool isKeyword(char* str)
{
    const char* keywords[] = 
        { "starto", "endo", "disp", "fetch", "craft", "yield", "quando", 
          "alt", "opt", "pro", "dum", "exec", "true", "false", "bind", 
          "flux", "set", "unset", "numbra", "deca", "signa", "duplos", 
          "formo", "binar", "goto" };
    int i;
    for (i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Function to check if a string is an integer
bool isInteger(char* str)
{
    if (str == NULL || *str == '\0') {
        return false;
    }
    int i = 0;
    while (isdigit(str[i])) {
        i++;
    }
    return str[i] == '\0';
}

// Function to get a substring of a string
char* getSubstring(char* str, int start, int end)
{
    int subLength = end - start + 1;
    char* subStr = (char*)malloc((subLength + 1) * sizeof(char));
    strncpy(subStr, str + start, subLength);
    subStr[subLength] = '\0';
    return subStr;
}

// Lexical Analyzer function
int lexicalAnalyzer(char* input)
{
    int left = 0, right = 0;
    int len = strlen(input);

    // Print the table headers
    printf("+------------------+---------------------+\n");
    printf("| Token Type       | Value               |\n");
    printf("+------------------+---------------------+\n");

    // Tokenize the input string
    while (right <= len && left <= right) {
        // Skip spaces (x=2+x / x = 2 + x)
        if (isspace(input[right])) {
            right++;
            left = right;
            continue;
        }

        // Check for line terminator
        if (input[right] == '/' && input[right + 1] == '/') {
            printf("| %-16s | %-19s |\n", "Line Terminator", "//");
            right += 2;
            left = right;
            continue;
        }

        // Handle string literals
        if (input[right] == '"') {
            right++;
            int start = right;
            while (input[right] != '"' && input[right] != '\0') {
                right++;
            }
            if (input[right] == '"') {
                char* subStr = getSubstring(input, start, right - 1);
                printf("| %-16s | %-19s |\n", "String Literal", subStr);
                free(subStr);
                right++; // Move past the closing quote
                left = right;
                continue;
            } else {
                printf("| %-16s | %-19s |\n", "Unterminated String", "");
                break; // Unterminated string
            }
        }

        // Handle delimiters
        if (isDelimiter(input[right])) {
            printf("| %-16s | %-19c |\n", "Delimiter", input[right]);
            right++;
            left = right;
            continue;
        }

        // Handle operators
        if (isOperator(input[right])) {
            printf("| %-16s | %-19c |\n", "Operator", input[right]);
            right++;
            left = right; // Reset pointers to avoid skipping subsequent tokens
            continue;
        }

        // Process substrings
        if (!isDelimiter(input[right]) && !isspace(input[right]) && input[right] != '\0') {
            right++;
        }

        if (isDelimiter(input[right]) || isspace(input[right]) || right == len) {
            char* subStr = getSubstring(input, left, right - 1);

            // Skip processing if substring is NULL or empty
            if (subStr == NULL || strlen(subStr) == 0) {
                left = ++right;
                free(subStr);
                continue;
            }

            if (isKeyword(subStr))
                printf("| %-16s | %-19s |\n", "Keyword", subStr);

            else if (isInteger(subStr))
                printf("| %-16s | %-19s |\n", "Integer", subStr);

            else if (isValidIdentifier(subStr))
                printf("| %-16s | %-19s |\n", "Identifier", subStr);

            else
                printf("| %-16s | %-19s |\n", "Unidentified", subStr);

            free(subStr);  // Free allocated memory
            left = right;
        }
    }

    // Print the table footer
    printf("+------------------+---------------------+\n");
    return 0;
}

// Main function	
int main()
{
    // Input
    char lex_input[MAX_LENGTH] = 
        "starto endo disp fetch craft yield quando "
		"alt opt pro dum exec true false bind "
		"flux set unset numbra deca signa duplos "
		"formo binar goto "
		"@age @Age @aGe 1 2 10 10.5 "
		"+ - = {} () <>* //";

    printf("For Expression \"%s\":\n", lex_input);
    lexicalAnalyzer(lex_input);
    printf("\n");

    return 0;
}

