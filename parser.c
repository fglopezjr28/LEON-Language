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

// Function to validate <declaration_stmt>
int validateDeclarationStmt(Token* tokens, int count, int* index) {
    int i = *index;

    // Check for <data_type>
    if (i < count && (
        strcmp(tokens[i].lexeme, "numbra") == 0 || 
        strcmp(tokens[i].lexeme, "deca") == 0 ||
        strcmp(tokens[i].lexeme, "duplos") == 0 ||
        strcmp(tokens[i].lexeme, "signa") == 0 ||
        strcmp(tokens[i].lexeme, "binar") == 0)) {

        i++; // Move to <var_list> or assignment
        
        // Check for VARIABLE
        if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
            i++; // Move to optional '=' or ','

            // Optional '=' assignment
            if (i < count && strcmp(tokens[i].tokenType, "ASSIGNMENT_OP") == 0 && strcmp(tokens[i].lexeme, "=") == 0) {
                i++; // Consume '='

                // Check for VARIABLE on the right-hand side
                if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
                    i++; // Assignment is valid; move to LINE_TERMINATOR
                } else {
                    return 0; // Invalid assignment if no VARIABLE follows '='
                }
            }

            // Handle optional ',' <var_list>
            while (i < count && strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, ",") == 0) {
                i++; // Consume ','

                if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
                    i++; // Move to next token
                } else {
                    return 0; // Invalid if no VARIABLE follows the ','
                }
            }

            // Check for LINE_TERMINATOR
            if (i < count && strcmp(tokens[i].tokenType, "LINE_TERMINATOR") == 0) {
                *index = i + 1;
                return 1; // Valid <declaration_stmt>
            }
        }
    }

    return 0; // Invalid
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

    if (validateDeclarationStmt(tokens, tokenCount, &index)) {
        printf("Valid <declaration_stmt>\n");
    } else {
        printf("Input does not match <declaration_stmt>\n");
    }

    return 0;
}
