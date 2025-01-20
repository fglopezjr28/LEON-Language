#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 1000
#define MAX_TOKENS 1000

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


// Function to check if the next token is a closing parenthesis
int checkClosingParenthesis(Token* tokens, int* index, int tokenCount) {
    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 &&
        strcmp(tokens[*index].lexeme, ")") == 0) {
        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
        (*index)++;
        return 1; // Successfully found closing parenthesis
    }
    return 0; // No closing parenthesis found
}

// Function to process variables after a comma
int processVariable(Token* tokens, int* index, int tokenCount) {
    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
        printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
        (*index)++;
        return 1; // Successfully processed variable
    }
    printf("Error: Expected variable\n");
    return 0; // No variable found
}

// Function to process a comma and expect the next token to be a variable
int processCommaAndExpectVariable(Token* tokens, int* index, int tokenCount) {
    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 &&
        strcmp(tokens[*index].lexeme, ",") == 0) {
        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print comma
        (*index)++;
        return processVariable(tokens, index, tokenCount); // Expect variable after comma
    }
    return 0; // No comma found
}

// Iterartive Statement parser
    //  for_statement
    // while_statement 
    // do_while_statement


// Conditional Statement parser


// Declaration Statement parser
void Declaration_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    // Check for KEYWORD token (data type)
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 &&
        (strcmp(tokens[*index].lexeme, "numbra") == 0 ||
         strcmp(tokens[*index].lexeme, "deca") == 0 || 
         strcmp(tokens[*index].lexeme, "duplus") == 0 || 
         strcmp(tokens[*index].lexeme, "signa") == 0 ||
         strcmp(tokens[*index].lexeme, "binar") == 0)) {
        printf("(DECLARATION_STATEMENT(KEYWORD(Data Type): %s)", tokens[*index].lexeme); // Print KEYWORD (data type)
        (*index)++;

        // Parse variables and assignments
        while (*index < tokenCount) {
            // Check for VARIABLE token
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
                (*index)++;

                // Check for ASSIGNMENT_OP and value
                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0) {
                    printf("(ASSIGNMENT_OP: %s)", tokens[*index].lexeme); // Print ASSIGNMENT_OP
                    (*index)++;

                    // Expect a literal value
                    if (*index < tokenCount &&
                        (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                         strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                         strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                         strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0)) {
                        printf("(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print value
                        (*index)++;
                    } else {
                        printf("Error: Expected a literal value after '='\n");
                        return;
                    }
                }

                // Check for comma or line terminator
                if (*index < tokenCount) {
                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ",") == 0) {
                        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print DELIMETER
                        (*index)++;
                    } else if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                        printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                        (*index)++;
                        return; // Successfully parsed the statement
                    } else {
                        printf("Error: Expected ',' or '//' after variable\n");
                        return;
                    }
                } else {
                    printf("Error: Incomplete declaration statement\n");
                    return;
                }
            } else {
                printf("Error: Expected variable name after data type\n");
                return;
            }
        }
    } else {
        printf("Error: Expected data type keyword\n");
    }
}


// Input statement parser
void Input_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    // Check for the INPUT_KEYWORD token (e.g., 'fetch')
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "fetch") == 0) {
        printf("(INPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print INPUT_KEYWORD
        (*index)++;

        // Check for opening parenthesis '('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for FORMAT_SPECIFIER token
            if (strcmp(tokens[*index].tokenType, "FORMAT_SPECIFIER") == 0) {
                printf("(FORMAT_SPECIFIER: %s)", tokens[*index].lexeme); // Print FORMAT_SPECIFIER
                (*index)++;

                // Check for comma and expect a variable
                if (processCommaAndExpectVariable(tokens, index, tokenCount)) {
                    // Check for closing parenthesis
                    if (!checkClosingParenthesis(tokens, index, tokenCount)) {
                        printf("Error: Expected closing parenthesis ')'\n");
                        return;
                    }

                    // Check for LINE_TERMINATOR token
                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                        printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                        (*index)++;
                    } else {
                        printf("Error: Expected line terminator (//)\n");
                    }
                } else {
                    printf("Error: Expected variable after comma\n");
                }
            } else {
                printf("Error: Expected format specifier\n");
            }
        } else {
            printf("Error: Expected opening parenthesis '('\n");
        }
    } else {
        printf("Error: Expected input keyword 'fetch'\n");
    }   
}

// Output statement parser
void Output_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    // Check for the KEYWORD token (must be "disp")
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
        printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
        (*index)++;

        // Check for the DELIMITER token (must be '(')
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Process the content inside the parentheses
            int processedSomething = 0;

            // Handle STRING_LITERAL token
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                (*index)++;
                processedSomething = 1;
            }
            // Handle FORMAT_SPECIFIER token
            else if (*index < tokenCount && strcmp(tokens[*index].tokenType, "FORMAT_SPECIFIER") == 0) {
                printf("(FORMAT_SPECIFIER: %s)", tokens[*index].lexeme); // Print FORMAT_SPECIFIER
                (*index)++;
                processedSomething = 1;
            }

            // Process any subsequent variables or commas
            while (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0) {
                if (!processCommaAndExpectVariable(tokens, index, tokenCount)) {
                    break; // No more commas or variables
                }
                processedSomething = 1;
            }

            // Check for closing parenthesis
            if (!checkClosingParenthesis(tokens, index, tokenCount)) {
                printf("Error: Expected closing parenthesis ')'\n");
                return;
            }

            // Check for the LINE_TERMINATOR token (e.g., "//")
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                (*index)++;
            } else {
                printf("Error: Expected line terminator (//)\n");
            }
        } else {
            printf("Error: Expected opening parenthesis '('\n");
        }
    } else {
        printf("Error: Expected keyword 'disp'\n");
    }
}


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

            // Handle the assignment's expected ending
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
        // Determine which statement to parse based on the token type
        if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && 
            strcmp(tokens[index].lexeme, "numbra") == 0 ||
            strcmp(tokens[index].lexeme, "deca") == 0 || 
            strcmp(tokens[index].lexeme, "duplus") == 0 || 
            strcmp(tokens[index].lexeme, "signa") == 0 ||
            strcmp(tokens[index].lexeme, "binar") == 0 ) {
            // "numbra", "deca", "duplus", "signa", "binar" keywords indicate the start of a declaration statement
            Declaration_Stmt(tokens, &index, tokenCount);
        } else if (strcmp(tokens[index].tokenType, "VARIABLE") == 0) {
            // Variables indicate the start of an assignment statement
            Assign_Stmt(tokens, &index, tokenCount);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "disp") == 0) {
            // "disp" keyword indicates the start of an output statement
            Output_Stmt(tokens, &index, tokenCount);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "fetch") == 0) {
            // "fetch" keyword indicates the start of an input statement
            Input_Stmt(tokens, &index, tokenCount);
        } else {
            // Handle unexpected tokens gracefully
            printf("Error: Unexpected token '%s' on line %d\n", tokens[index].lexeme, tokens[index].line);
            index++; // Skip the unrecognized token
        }
    }

    return 0;
}
