#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100
#define MAX_TOKENS 100

// Token structure to store token type, lexeme, and line number
typedef struct {
    char tokenType[MAX_TOKEN_LENGTH];
    char lexeme[MAX_TOKEN_LENGTH];
    int line;
} Token;

// Function to read tokens from the input file (symbol table)
int readTokens(FILE* file, Token* tokens) {
    int count = 0;
    char line[MAX_TOKEN_LENGTH];
    
    // Skip header line
    fgets(line, sizeof(line), file); 
    
    while (fgets(line, sizeof(line), file)) {
        // Parse the token type, lexeme, and line number
        if (sscanf(line, "| %s | %s | %d |", tokens[count].tokenType, tokens[count].lexeme, &tokens[count].line) == 3) {
            count++;
        }
    }
    return count;
}

// Lookahead function to peek at the next token
const char* look_ahead(Token* tokens, int index) {
    return (index + 1 < MAX_TOKENS) ? tokens[index + 1].tokenType : NULL;
}

//Input statement parser


// Assign statement parser
void Assign_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    // Check for the VARIABLE token (e.g., @age)
    if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
        printf("(ASSIGNMENT_STATEMENT(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
        (*index)++;

        // Check for the ASSIGNMENT_OP token (e.g., '=')
        if (strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0) {
            printf("(ASSIGNMENT_OP: %s)", tokens[*index].lexeme); // Print ASSIGNMENT_OP
            (*index)++;

            // Handle the right-hand side of the assignment
            while (*index < tokenCount && 
                   (strcmp(tokens[*index].tokenType, "VARIABLE") == 0 ||
                    strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                    strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 ||
                    strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0)) {

                // Handle literals, variables, and operators
                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE (right-hand side)
                    (*index)++;
                } else if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                    printf("(INT_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                    (*index)++;
                } else if (strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0) {
                    printf("(FLOAT_LITERAL: %s)", tokens[*index].lexeme); // Print FLOAT_LITERAL
                    (*index)++;
                } else if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                    printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print ARITHMETIC_OP
                    (*index)++;
                }
            }

            // Check for the LINE_TERMINATOR token (e.g., "//")
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR and close the statement
                (*index)++;
            } else {
                printf("Error: Expected line terminator (//)\n");
            }
        } else {
            printf("Error: Expected assignment operator (=)\n");
        }
    } else {
        printf("Error: Expected a variable\n");
    }
}

int main() {
    FILE* file = fopen("symboltable1.leon", "r");
    if (!file) {
        printf("Error: Could not open symboltable1.leon\n");
        return 1;
    }

    Token tokens[MAX_TOKENS];
    int tokenCount = readTokens(file, tokens);
    fclose(file);

    int index = 0;

    while (index < tokenCount) {
        Assign_Stmt(tokens, &index, tokenCount);
    }

    return 0;
}
