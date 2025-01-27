#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 1000
#define MAX_TOKENS 1000

// Token structure to store token type, lexeme, and line number
typedef struct {
    char tokenType[MAX_TOKEN_LENGTH];
    char lexeme[MAX_TOKEN_LENGTH];
    int line;
} Token;

int readTokens(FILE* file, Token* tokens) {
    int count = 0;
    char line[MAX_TOKEN_LENGTH];
    
    // Skip header line
    fgets(line, sizeof(line), file); 
    
    while (fgets(line, sizeof(line), file)) {
        // Parse the token type, lexeme, and line number
        if (sscanf(line, "| %s | %[^|] | %d |", tokens[count].tokenType, tokens[count].lexeme, &tokens[count].line) == 3) {
            // Remove trailing spaces from lexeme
            char* end = tokens[count].lexeme + strlen(tokens[count].lexeme) - 1;
            while (end > tokens[count].lexeme && isspace((unsigned char)*end)) {
                end--;
            }
            end[1] = '\0';
            count++;
        }
    }
    return count;
}

// Lookahead function to peek at the next token
const char* look_ahead(Token* tokens, int index) {
    return (index + 1 < MAX_TOKENS) ? tokens[index + 1].tokenType : NULL;
}

// Function to process variables after a comma
int processVariable(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
        fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
        (*index)++;
        return 1; // Successfully processed variable
    }
    fprintf(outputFile, "Error: Expected variable\n");
    return 0; // No variable found
}

// Function to process a comma and expect the next token to be a variable
int processCommaAndExpectVariable(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 &&
        strcmp(tokens[*index].lexeme, ",") == 0) {
        fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print comma
        (*index)++;
        return processVariable(tokens, index, tokenCount, outputFile); // Expect variable after comma
    }
    return 0; // No comma found
}

// Output statement parser
void Output_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    // Check for the KEYWORD token (must be "disp")
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
        fprintf(outputFile, "(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
        (*index)++;

        // Check for the DELIMITER token (must be '(')
        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Process the content inside the parentheses
            int processedSomething = 0;

            // Handle STRING_LITERAL token
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                fprintf(outputFile, "(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                (*index)++;
                processedSomething = 1;
            }
            // Handle FORMAT_SPECIFIER token
            else if (*index < tokenCount && strcmp(tokens[*index].tokenType, "FORMAT_SPECIFIER") == 0) {
                fprintf(outputFile, "(FORMAT_SPECIFIER: %s)", tokens[*index].lexeme); // Print FORMAT_SPECIFIER
                (*index)++;
                processedSomething = 1;
            }

            // Process any subsequent variables or commas
            while (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0) {
                if (!processCommaAndExpectVariable(tokens, index, tokenCount, outputFile)) {
                    break; // No more commas or variables
                }
                processedSomething = 1;
            }

            // Check for closing parenthesis
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                (*index)++;
            } else {
                fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                return;
            }

            // Check for the LINE_TERMINATOR token (e.g., "//")
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                fprintf(outputFile, "(LINE_TERMINATOR: %s))\n\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                (*index)++;
            } else {
                fprintf(outputFile, "Error: Expected line terminator (//)\n");
            }
        } else {
            fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
        }
    } else {
        fprintf(outputFile, "Error: Expected keyword 'disp'\n");
    }
}

// Iterartive Statement parser
void Iterative_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    //  for_statement Check for the KEYWORD token (must be "pro")
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "pro") == 0) {
        fprintf(outputFile, "(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print pro
        (*index)++;

        // Check for opening parenthesis 'pro ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for opening parenthesis 'pro ( data_type'
            if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "numbra") == 0 ||
                strcmp(tokens[*index].lexeme, "deca") == 0 || 
                strcmp(tokens[*index].lexeme, "duplus") == 0 || 
                strcmp(tokens[*index].lexeme, "signa") == 0 ||
                strcmp(tokens[*index].lexeme, "binar") == 0) {
                fprintf(outputFile, "(KEYWORD: %s)", tokens[*index].lexeme); // Print data_type
                (*index)++;

                // Check for variable 'pro ( data_type variable'
                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                    fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                    (*index)++;

                    // Check for assignment operator 'pro ( data_type variable ='
                    if (strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0 && strcmp(tokens[*index].lexeme, "=") == 0) {
                        fprintf(outputFile, "(ASSIGNMENT_OP: %s)", tokens[*index].lexeme); // Print assignment operator
                        (*index)++;

                        // Check for INTEGER_LITERAL token 'pro ( data_type variable = INTEGER_LITERAL'
                        if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                            fprintf(outputFile, "(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                            (*index)++;

                            // Check for semicolon 'pro ( data_type variable = INTEGER_LITERAL ;'
                             if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ";") == 0) {
                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print semicolon
                                (*index)++;

                                // Check for variable 'pro ( data_type variable = INTEGER_LITERAL ; variable'
                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                    fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                    (*index)++;

                                    // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable relational_op'
                                    if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                        fprintf(outputFile, "(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                                        (*index)++;

                                        // Check for INTEGER_LITERAL token 'pro ( data_type variable = INTEGER_LITERAL ; variable relational_op INTEGER_LITERAL'
                                        if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                                            fprintf(outputFile, "(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                                            (*index)++;

                                            // Check for semicolon 'pro ( data_type variable = INTEGER_LITERAL ;'
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ";") == 0) {
                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print semicolon
                                                (*index)++;

                                                // Check for variable 'pro ( data_type variable = INTEGER_LITERAL ; variable'
                                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                    fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                    (*index)++;

                                                    // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable +'
                                                    if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                        fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                        (*index)++;

                                                        // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable ++'
                                                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                            fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                            (*index)++;

                                                            // Check for delimeter 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ )'
                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                (*index)++;

                                                                // Check for opening curly braces 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) {'
                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                    fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
                                                                    (*index)++;

                                                                    Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                                                                        // Check for closing curly braces 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp (string_literal) // }'
                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                            fprintf(outputFile, "(DELIMETER: %s))\n\n", tokens[*index].lexeme); // Print closing curly braces
                                                                            (*index)++;
                                                                        } else { 
                                                                            fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                                                        }
                                                                } else {
                                                                    fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                                                                }
                                                            } else {
                                                                fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                                                            }
                                                        } else {
                                                            fprintf(outputFile, "Error: Expected arithmetic operator\n");
                                                        }
                                                    } else {
                                                        fprintf(outputFile, "Error: Expected arithmetic operator\n");
                                                    }
                                                } else {
                                                    fprintf(outputFile, "Error: Expected variable\n");
                                                }
                                            } else {
                                                fprintf(outputFile, "Error: Expected semicolon ';'\n");
                                            }
                                        } else {
                                            fprintf(outputFile, "Error: Expected integer literal\n");
                                        }
                                    } else {
                                        fprintf(outputFile, "Error: Expected relational operator\n");
                                    }
                                } else {
                                    fprintf(outputFile, "Error: Expected variable\n");
                                }
                             } else {
                                fprintf(outputFile, "Error: Expected semicolon ';'\n");
                             }
                        } else {
                            fprintf(outputFile, "Error: Expected integer literal\n");
                        }
                    } else {
                        fprintf(outputFile, "Error: Expected assignment operator '='\n");
                    }
                } else {
                    fprintf(outputFile, "Error: Expected variable\n");
                }
            } else {
                fprintf(outputFile, "Error: Expected data type keyword\n");
            }

        } else {
            fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
        }
    }

    // while_statement Check for the KEYWORD token (must be "dum")
    else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "dum") == 0) {
        fprintf(outputFile, "(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
        (*index)++;

        // Check for opening parenthesis 'dum ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for variable 'dum (variable'
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                (*index)++;

                // Check for relational operator 'dum (variable relational_op'
                if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                    fprintf(outputFile, "(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                    (*index)++;

                    // Check for INTEGER_LITERAL token 'dum (variable relational_op INTEGER_LITERAL'
                    if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                        fprintf(outputFile, "(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                        (*index)++;

                        // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL)'
                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                            (*index)++;

                            // Check for opening curly braces 'dum (variable relational_op INTEGER_LITERAL) {'
                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
                                (*index)++;

                                Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                                    // Check for variable 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable}'
                                    if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                        fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                        (*index)++;

                                        // Check for assignment operator 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable +'
                                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                            fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                            (*index)++;

                                            // Check for assignment operator 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable ++'
                                            if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                (*index)++;

                                                // check for line terminator 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable ++ //'
                                                if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                    fprintf(outputFile, "(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                                    (*index)++;

                                                    // Check for closing curly braces 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable ++ // }'
                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                        fprintf(outputFile, "(DELIMETER: %s)\n\n", tokens[*index].lexeme); // Print opening curly braces
                                                        (*index)++;
                                                    } else {
                                                        fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                                    }
                                                } else {
                                                    fprintf(outputFile, "Error: Expected line terminator '//'\n");
                                                }
                                            } else {
                                                fprintf(outputFile, "Error: Expected arithmetic operator\n");
                                            }
                                        } else {
                                            fprintf(outputFile, "Error: Expected arithmetic operator\n");
                                        }   
                                    } else {
                                        fprintf(outputFile, "Error: Expected variable\n");
                                    }
                            } else {
                                fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                            }
                        } else {
                            fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                        }
                    } else {
                        fprintf(outputFile, "Error: Expected integer literal\n");
                    }
                } else {
                    fprintf(outputFile, "Error: Expected relational operator\n");
                }
            } else {
                fprintf(outputFile, "Error: Expected variable\n");
            }
        } else {
            fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
        }
    }

    // do_while_statement Check for the KEYWORD token (must be "exec")
    else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "exec") == 0) {
        fprintf(outputFile, "(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
        (*index)++;

        // Check for opening curly braces 'exec {'
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
            (*index)++;

            Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                // Check for 'exec { disp (string_literal) // variable'
                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                    fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                    (*index)++;

                    // Check for assignment operator 'exec { disp (string_literal) // variable +'
                    if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                        fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                        (*index)++;

                        // Check for assignment operator 'exec { disp (string_literal) // variable ++'
                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                            fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                            (*index)++;

                            // Check for opening curly braces 'exec { disp (string_literal) // variable ++ //'
                            if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                fprintf(outputFile, "(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print line terminator
                                (*index)++;

                                // Check for closing curly braces 'exec { disp (string_literal) // variable ++ // }'
                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                    fprintf(outputFile, "(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                    (*index)++;

                                    // Check for keyword 'dum'
                                    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "dum") == 0) {
                                        fprintf(outputFile, "(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
                                        (*index)++;

                                            // Check for opening parenthesis 'dum ('
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                (*index)++;

                                                // Check for data_type 'dum ( data_type'
                                                if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "numbra") == 0 ||
                                                    strcmp(tokens[*index].lexeme, "deca") == 0 || 
                                                    strcmp(tokens[*index].lexeme, "duplus") == 0 || 
                                                    strcmp(tokens[*index].lexeme, "signa") == 0 ||
                                                    strcmp(tokens[*index].lexeme, "binar") == 0) {
                                                    fprintf(outputFile, "(KEYWORD: %s)", tokens[*index].lexeme); // Print data_type
                                                    (*index)++;

                                                    // Check for variable 'dum ( data_type variable'
                                                    if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                        fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                        (*index)++;

                                                        // Check for relational operator 'dum (variable relational_op'
                                                        if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                                            fprintf(outputFile, "(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                                                            (*index)++;

                                                            // Check for INTEGER_LITERAL token 'dum (variable relational_op INTEGER_LITERAL'
                                                            if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                                                                fprintf(outputFile, "(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                                                                (*index)++;

                                                                // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL)'
                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                    fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                    (*index)++;

                                                                    // Check for opening curly braces 'dum (variable relational_op INTEGER_LITERAL) //'
                                                                    if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                                        fprintf(outputFile, "(LINE_TERMINATOR: %s)\n", tokens[*index].lexeme); // Print line terminator
                                                                        (*index)++;
                                                                    } else {
                                                                        fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                                                                    }
                                                                } else {
                                                                    fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                                                                }
                                                            } else {
                                                                fprintf(outputFile, "Error: Expected integer literal\n");
                                                            }
                                                        } else {
                                                            fprintf(outputFile, "Error: Expected relational operator\n");
                                                        }
                                                    } else {
                                                        fprintf(outputFile, "Error: Expected variable\n");
                                                    }
                                                } else {
                                                    fprintf(outputFile, "Error: Expected data type keyword\n");
                                                }
                                            } else {
                                                fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
                                            }
                                        } else {
                                            fprintf(outputFile, "Error: Expected keyword 'dum'\n");
                                        }
                                    } else {
                                        fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                    }
                                } else {
                                    fprintf(outputFile, "Error: Expected line terminator (//)\n");
                                }
                            } else {
                                fprintf(outputFile, "Error: Expected arithmetic operator\n");
                            }
                        } else {
                            fprintf(outputFile, "Error: Expected arithmetic operator\n");
                        }
                    } else {
                        fprintf(outputFile, "Error: Expected variable\n");
                    }
        } else {
            fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
        }
    } else {
        fprintf(outputFile, "Error: Expected iterative keyword 'pro', 'dum', or 'exec'\n");
    }                                          
}
    
// Conditional Statement parser
void Conditional_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    // Check for the INPUT_KEYWORD token (e.g., 'quando')
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "quando") == 0) {
        fprintf(outputFile, "(CONDITIONAL_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
        (*index)++;

        // Check for opening parenthesis 'quando ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Expect a variable 'quando (variable'
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                (*index)++;

                // Expect Relational OP 'quando (variable relational_op'
                if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                    fprintf(outputFile, "(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print variable
                    (*index)++;

                    // Check for init value 'quando (variable relational_op init_value'
                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                        strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                        strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                        strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0 ) {
                        fprintf(outputFile, "(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print init_value
                        (*index)++;

                        // Check for closing parenthesis ')' 'quando (variable relational_op init_value)'
                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                            (*index)++;
                                
                            // Check for opening curly braces 'quando (variable relational_op init_value) {' 
                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                (*index)++;

                                Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                                    // Check for closing curly braces 'quando (variable relational_op init_value) { disp (string_literal) // }'
                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                        fprintf(outputFile, "(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                        (*index)++;

                                        // Check for the KEYWORD token (must be "opt")  'opt    
                                        if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "opt") == 0) {
                                            fprintf(outputFile, "(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                            (*index)++;

                                            // Check for opening parenthesis 'opt ('
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                (*index)++;

                                                // Expect a variable 'opt (variable'
                                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                    fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                    (*index)++;
                                                                            
                                                    // Expect Relational OP 'opt (variable relational_op'
                                                    if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                                        fprintf(outputFile, "(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print variable
                                                        (*index)++;

                                                        // Check for init value 'opt (variable relational_op init_value'
                                                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                                                            strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                                                            strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                                                            strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0 ) {
                                                            fprintf(outputFile, "(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print init_value
                                                            (*index)++;

                                                            // Check for closing parenthesis 'opt (variable relational_op init_value)'
                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                (*index)++;

                                                                // Check for opening curly braces 'opt (variable relational_op init_value) {'
                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                    fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                    (*index)++;

                                                                    Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement
                                                                                                                            
                                                                        // Check for 'opt (variable relational_op init_value) { disp(STRING_LITERAL) // }'
                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                        fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing curly braces
                                                                        (*index)++;

                                                                            // Check for 'alt'
                                                                            if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "alt") == 0) {
                                                                                fprintf(outputFile, "(CONDITIONAL_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
                                                                                (*index)++;

                                                                                // Check for 'alt {'
                                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                                    fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                    (*index)++;

                                                                                    Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                                                                                        // Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                                            fprintf(outputFile, "(DELIMETER: %s)\n\n", tokens[*index].lexeme); // Print closing curly braces
                                                                                            (*index)++;
                                                                                        } else { // Error Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                            fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                                                                        }
                                                                                                                                                                                           
                                                                                } else { // Error Check for 'alt {'
                                                                                    fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                                                                                }
                                                                            } else { // Error Check for 'alt'
                                                                                fprintf(outputFile, "Error: Expected conditional keyword 'alt'\n");
                                                                            }
                                                                        } else { // Error Check for 'opt (variable relational_op init_value) { disp (string_literal) // }'
                                                                            fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                                                        }
                                                                } else { // Error Check for 'opt (variable relational_op init_value) {'
                                                                    fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                                                                }
                                                            } else { // Error Check for 'opt (variable relational_op init_value)'
                                                                fprintf(outputFile, "Error: Expected closing parenthesis\n");
                                                            }
                                                        } else { // Error Check for 'opt (variable relational_op init_value'
                                                            fprintf(outputFile, "Error: Expected init_value\n");
                                                        }
                                                    } else { // Error Check for 'opt (variable relational_op'
                                                        fprintf(outputFile, "Error: Expected relational_op\n");
                                                    }
                                                } else { // Error Check for 'opt (variable'
                                                    fprintf(outputFile, "Error: Expected variable\n");
                                                }
                                            } else { // Error Check for 'opt ('
                                                fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
                                            }

                                        // If no opt encountered check for 'alt'
                                        } else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "alt") == 0) { 
                                            fprintf(outputFile, "(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
                                            (*index)++;

                                            // Check for opening curly braces 'alt {'
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                (*index)++;

                                                Output_Stmt(tokens, index, tokenCount, outputFile); // Handle the disp statement

                                                    // Check for 'alt { disp(STRING_LITERAL) // }'
                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                        fprintf(outputFile, "(DELIMETER: %s)\n\n", tokens[*index].lexeme); // Print closing curly braces
                                                        (*index)++;
                                                    } else { // Error Check for 'alt { disp(STRING_LITERAL) // }'
                                                        fprintf(outputFile, "Error: Expected closing curly braces '}'\n");
                                                    }
                                            } else { // Error Check for 'alt {'
                                                fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                                            }
                                        } else { // Error Check for 'opt' or 'alt
                                            fprintf(outputFile, "\n");
                                        }

                                    } else { // Error Check for 'quando (variable relational_op init_value) { disp (string_literal) // }'
                                        fprintf(outputFile, "");
                                    }
                            } else { // Error Check for 'quando (variable relational_op init_value) {'
                                fprintf(outputFile, "Error: Expected opening curly braces '{'\n");
                            }
                        } else { // Error Check for 'quando (variable relational_op init_value)'
                            fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                        }
                    } else { // Error Check for 'quando (variable relational_op init_value'
                        fprintf(outputFile, "Error: Expected init_value\n");
                    }
                } else { // Error Check for 'quando (variable relational_op'
                    fprintf(outputFile, "Error: Expected relational operator\n");
                }
            } else { // Error Check for 'quando (variable'
                fprintf(outputFile, "Error: Expected variable\n");
            }
        } else { // Error check for opening parenthesis 'quando ('
            fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
        }
    } else { // Error check for the INPUT_KEYWORD token (e.g., 'quando')
        fprintf(outputFile, "Error: Expected conditional keyword 'quando'\n");
    }
}


// Declaration Statement parser
void Declaration_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    // Check for a valid data type keyword
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 &&
        (strcmp(tokens[*index].lexeme, "numbra") == 0 || 
         strcmp(tokens[*index].lexeme, "deca") == 0 || 
         strcmp(tokens[*index].lexeme, "duplus") == 0 || 
         strcmp(tokens[*index].lexeme, "signa") == 0 || 
         strcmp(tokens[*index].lexeme, "binar") == 0)) {
        fprintf(outputFile, "(DECLARATION_STATEMENT(KEYWORD(Data Type): %s)", tokens[*index].lexeme);
        (*index)++;

        // Parse the variable list and optional assignments
        int expectComma = 0; // Track whether we expect a comma next

        while (*index < tokenCount) {
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                // Print variable and proceed
                fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme);
                (*index)++;
                expectComma = 1; // After a variable, expect a comma or other tokens
            } else if (strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0) {
                // Handle assignment to a variable
                fprintf(outputFile, "(ASSIGNMENT_OP: %s)", tokens[*index].lexeme);
                (*index)++;

                // Expect an initialization value
                if (*index < tokenCount &&
                    (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                     strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 ||
                     strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 ||
                     strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0)) {
                    fprintf(outputFile, "(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme);
                    (*index)++;
                } else {
                    fprintf(outputFile, "Error: Expected an initialization value after '='\n");
                    return;
                }
                expectComma = 1; // After an assignment, expect a comma or other tokens
            } else if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 &&
                       strcmp(tokens[*index].lexeme, ",") == 0) {
                // Handle a comma between variables
                fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme);
                (*index)++;
                expectComma = 0; // After a comma, expect a variable or assignment
            } else if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                // Handle the line terminator and close the statement
                fprintf(outputFile, "(LINE_TERMINATOR: %s))\n\n", tokens[*index].lexeme);
                (*index)++;
                return;
            } else {
                fprintf(outputFile, "Error: Unexpected token '%s' on line %d\n",
                        tokens[*index].lexeme, tokens[*index].line);
                return;
            }
        }

        if (expectComma) {
            fprintf(outputFile, "Error: Statement ends unexpectedly after ','\n");
        }
    } else {
        fprintf(outputFile, "Error: Expected a data type keyword\n");
    }
}

// Input statement parser
void Input_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    // Check for the INPUT_KEYWORD token (e.g., 'fetch')
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "fetch") == 0) {
        fprintf(outputFile, "(INPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print INPUT_KEYWORD
        (*index)++;

        // Check for opening parenthesis '('
        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for FORMAT_SPECIFIER token
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "FORMAT_SPECIFIER") == 0) {
                fprintf(outputFile, "(FORMAT_SPECIFIER: %s)", tokens[*index].lexeme); // Print FORMAT_SPECIFIER
                (*index)++;

                // Check for comma
                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ",") == 0) {
                    fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print comma
                    (*index)++;

                    // Check for VARIABLE token
                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                        fprintf(outputFile, "(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
                        (*index)++;

                        // Check for closing parenthesis ')'
                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                            fprintf(outputFile, "(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                            (*index)++;

                            // Check for LINE_TERMINATOR token
                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                fprintf(outputFile, "(LINE_TERMINATOR: %s))\n\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                (*index)++;
                            } else {
                                fprintf(outputFile, "Error: Expected line terminator (//)\n");
                            }
                        } else {
                            fprintf(outputFile, "Error: Expected closing parenthesis ')'\n");
                        }
                    } else {
                        fprintf(outputFile, "Error: Expected variable after comma\n");
                    }
                } else {
                    fprintf(outputFile, "Error: Expected comma ',' after format specifier\n");
                }
            } else {
                fprintf(outputFile, "Error: Expected format specifier\n");
            }
        } else {
            fprintf(outputFile, "Error: Expected opening parenthesis '('\n");
        }
    } else {
        fprintf(outputFile, "Error: Expected input keyword 'fetch'\n");
    }
}

// Assign statement parser
void Assign_Stmt(Token* tokens, int* index, int tokenCount, FILE* outputFile) {
    if (*index >= tokenCount) return;

    // Check for the VARIABLE token (e.g., @age)
    if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
        fprintf(outputFile, "(ASSIGNMENT_STATEMENT(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
        (*index)++;

        // Check for the ASSIGNMENT_OP token (e.g., '=')
        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0) {
            fprintf(outputFile, "(ASSIGNMENT_OP: %s)", tokens[*index].lexeme); // Print ASSIGNMENT_OP
            (*index)++;

            // Validate the right-hand side of the assignment
            int expectOperand = 1; // Flag to ensure proper operator-operand alternation

            while (*index < tokenCount) {
                if (expectOperand &&
                   (strcmp(tokens[*index].tokenType, "VARIABLE") == 0 ||
                    strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                    strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0)) {
                    // Valid operand
                    fprintf(outputFile, "(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme);
                    (*index)++;
                    expectOperand = 0; // Next, expect an operator or a line terminator
                } else if (!expectOperand && strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                    // Valid operator
                    fprintf(outputFile, "(ARITHMETIC_OP: %s)", tokens[*index].lexeme);
                    (*index)++;
                    expectOperand = 1; // Next, expect an operand
                } else if (!expectOperand && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                    // End of a valid assignment statement
                    fprintf(outputFile, "(LINE_TERMINATOR: %s))\n\n", tokens[*index].lexeme);
                    (*index)++;
                    return;
                } else {
                    // Invalid token sequence
                    fprintf(outputFile, "Error: Unexpected token '%s' in assignment statement\n", tokens[*index].lexeme);
                    return;
                }
            }

            // If we exit the loop without a LINE_TERMINATOR, it is an error
            fprintf(outputFile, "Error: Expected line terminator (//) at the end of assignment\n");
        } else {
            fprintf(outputFile, "Error: Expected assignment operator (=)\n");
        }
    } else {
        fprintf(outputFile, "Error: Expected a variable\n");
    }
}

int main() {
    FILE* inputFile = fopen("symboltable1.leon", "r");
    if (!inputFile) {
        printf("Error: Could not open symboltable1.leon\n");
        return 1;
    }

    FILE* outputFile = fopen("parseTree.leon", "w");
    if (!outputFile) {
        printf("Error: Could not create parseTree.leon\n");
        fclose(inputFile);
        return 1;
    }

    Token tokens[MAX_TOKENS];
    int tokenCount = readTokens(inputFile, tokens);
    fclose(inputFile);

    int index = 0;

    while (index < tokenCount) {
        if (strcmp(tokens[index].tokenType, "VARIABLE") == 0) {
            Assign_Stmt(tokens, &index, tokenCount, outputFile);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "disp") == 0) {
            Output_Stmt(tokens, &index, tokenCount, outputFile);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "fetch") == 0) {
            Input_Stmt(tokens, &index, tokenCount, outputFile);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && 
            (strcmp(tokens[index].lexeme, "numbra") == 0 || 
             strcmp(tokens[index].lexeme, "deca") == 0 || 
             strcmp(tokens[index].lexeme, "duplus") == 0 || 
             strcmp(tokens[index].lexeme, "signa") == 0 || 
             strcmp(tokens[index].lexeme, "binar") == 0)) {
            Declaration_Stmt(tokens, &index, tokenCount, outputFile);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "quando") == 0) {
            Conditional_Stmt(tokens, &index, tokenCount, outputFile);
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && 
            strcmp(tokens[index].lexeme, "pro") == 0 ||
            strcmp(tokens[index].lexeme, "dum") == 0 ||
            strcmp(tokens[index].lexeme, "exec") == 0) {
            Iterative_Stmt(tokens, &index, tokenCount, outputFile);
        }
        else {
            fprintf(outputFile, "Error: Unexpected token '%s' on line %d\n", tokens[index].lexeme, tokens[index].line);
            index++;
        }
    }

    printf("Parsing completed. Check parseTree.leon for the output.\n");

    fclose(outputFile);
    return 0;
}
