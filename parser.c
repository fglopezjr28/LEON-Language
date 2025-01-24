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
void Iterative_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    //  for_statement Check for the KEYWORD token (must be "pro")
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "pro") == 0) {
        printf("(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print pro
        (*index)++;

        // Check for opening parenthesis 'pro ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for opening parenthesis 'pro ( data_type'
            if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "numbra") == 0 ||
                strcmp(tokens[*index].lexeme, "deca") == 0 || 
                strcmp(tokens[*index].lexeme, "duplus") == 0 || 
                strcmp(tokens[*index].lexeme, "signa") == 0 ||
                strcmp(tokens[*index].lexeme, "binar") == 0) {
                printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print data_type
                (*index)++;

                // Check for variable 'pro ( data_type variable'
                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                    (*index)++;

                    // Check for assignment operator 'pro ( data_type variable ='
                    if (strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0 && strcmp(tokens[*index].lexeme, "=") == 0) {
                        printf("(ASSIGNMENT_OP: %s)", tokens[*index].lexeme); // Print assignment operator
                        (*index)++;

                        // Check for INTEGER_LITERAL token 'pro ( data_type variable = INTEGER_LITERAL'
                        if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                            printf("(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                            (*index)++;

                            // Check for semicolon 'pro ( data_type variable = INTEGER_LITERAL ;'
                             if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ";") == 0) {
                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print semicolon
                                (*index)++;

                                // Check for variable 'pro ( data_type variable = INTEGER_LITERAL ; variable'
                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                    (*index)++;

                                    // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable relational_op'
                                    if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                        printf("(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                                        (*index)++;

                                        // Check for INTEGER_LITERAL token 'pro ( data_type variable = INTEGER_LITERAL ; variable relational_op INTEGER_LITERAL'
                                        if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                                            printf("(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                                            (*index)++;

                                            // Check for semicolon 'pro ( data_type variable = INTEGER_LITERAL ;'
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ";") == 0) {
                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print semicolon
                                                (*index)++;

                                                // Check for variable 'pro ( data_type variable = INTEGER_LITERAL ; variable'
                                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                    (*index)++;

                                                    // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable +'
                                                    if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                        printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                        (*index)++;

                                                        // Check for relational operator 'pro ( data_type variable = INTEGER_LITERAL ; variable ++'
                                                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                            printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                            (*index)++;

                                                            // Check for delimeter 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ )'
                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                (*index)++;

                                                                // Check for opening curly braces 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) {'
                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
                                                                    (*index)++;

                                                                    // Check for the KEYWORD token (must be "disp") 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp'
                                                                    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                                                        printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                                                        (*index)++;

                                                                        // Check for opening parenthesis  'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp ('
                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                                            (*index)++;

                                                                            // Handle STRING_LITERAL token 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp (string_literal'
                                                                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                                                                printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                                                                (*index)++;

                                                                                // Check for closing parenthesis 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp (string_literal) //'
                                                                                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                                                    printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                                                    (*index)++;

                                                                                    // Check for closing curly braces 'pro ( data_type variable = INTEGER_LITERAL ; variable ++ ) { disp (string_literal) // }'
                                                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                                        printf("(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                                                                        (*index)++;
                                                                                    } else { 
                                                                                        printf("Error: Expected closing curly braces '}'\n");
                                                                                    }
                                                                                } else {
                                                                                    printf("Error: Expected line terminator (//)\n");
                                                                                }
                                                                            } else {
                                                                                printf("Error: Expected string literal\n");
                                                                            }
                                                                        } else {
                                                                            printf("Error: Expected opening parenthesis '('\n");
                                                                        }
                                                                    } else {
                                                                        printf("Error: Expected output keyword 'disp'\n");
                                                                    }
                                                                } else {
                                                                    printf("Error: Expected opening curly braces '{'\n");
                                                                }
                                                            } else {
                                                                printf("Error: Expected closing parenthesis ')'\n");
                                                            }
                                                        } else {
                                                            printf("Error: Expected arithmetic operator\n");
                                                        }
                                                    } else {
                                                        printf("Error: Expected arithmetic operator\n");
                                                    }
                                                } else {
                                                    printf("Error: Expected variable\n");
                                                }
                                            } else {
                                                printf("Error: Expected semicolon ';'\n");
                                            }
                                        } else {
                                            printf("Error: Expected integer literal\n");
                                        }
                                    } else {
                                        printf("Error: Expected relational operator\n");
                                    }
                                } else {
                                    printf("Error: Expected variable\n");
                                }
                             } else {
                                printf("Error: Expected semicolon ';'\n");
                             }
                        } else {
                            printf("Error: Expected integer literal\n");
                        }
                    } else {
                        printf("Error: Expected assignment operator '='\n");
                    }
                } else {
                    printf("Error: Expected variable\n");
                }
            } else {
                printf("Error: Expected data type keyword\n");
            }

        } else {
            printf("Error: Expected opening parenthesis '('\n");
        }
    }

    // while_statement Check for the KEYWORD token (must be "dum")
    else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "dum") == 0) {
        printf("(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
        (*index)++;

        // Check for opening parenthesis 'dum ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Check for variable 'dum (variable'
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                (*index)++;

                // Check for relational operator 'dum (variable relational_op'
                if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                    printf("(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                    (*index)++;

                    // Check for INTEGER_LITERAL token 'dum (variable relational_op INTEGER_LITERAL'
                    if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                        printf("(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                        (*index)++;

                        // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL)'
                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                            (*index)++;

                            // Check for opening curly braces 'dum (variable relational_op INTEGER_LITERAL) {'
                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
                                (*index)++;

                                // Check for the KEYWORD token (must be "disp") 'dum (variable relational_op INTEGER_LITERAL) { disp'
                                if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                    printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                    (*index)++;

                                    // Check for opening parenthesis  'dum (variable relational_op INTEGER_LITERAL) { disp ('
                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                        (*index)++;

                                        // Handle STRING_LITERAL token 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal'
                                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                            printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                            (*index)++;

                                            // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal)'
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                (*index)++;

                                                // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) //'
                                                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                    printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                    (*index)++;

                                                    // Check for variable 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable}'
                                                    if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                        printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                        (*index)++;

                                                        // Check for assignment operator 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable +'
                                                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                            printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                            (*index)++;

                                                            // Check for assignment operator 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable ++'
                                                            if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                                                printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                                                (*index)++;

                                                                // Check for opening curly braces 'dum (variable relational_op INTEGER_LITERAL) { disp (string_literal) // variable ++ }'
                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
                                                                    (*index)++;
                                                                } else {
                                                                    printf("Error: Expected closing curly braces '}'\n");
                                                                }
                                                            } else {
                                                                printf("Error: Expected arithmetic operator\n");
                                                            }
                                                        } else {
                                                            printf("Error: Expected arithmetic operator\n");
                                                        }   
                                                    } else {
                                                        printf("Error: Expected variable\n");
                                                    }
                                                } else {
                                                    printf("Error: Expected line terminator (//)\n");
                                                }
                                            } else {
                                                printf("Error: Expected closing parenthesis ')'\n");
                                            }
                                        } else {
                                            printf("Error: Expected string literal\n");
                                        }
                                    } else {
                                        printf("Error: Expected opening parenthesis '('\n");
                                    }
                                } else {
                                    printf("Error: Expected output keyword 'disp'\n");
                                }
                            } else {
                                printf("Error: Expected opening curly braces '{'\n");
                            }
                        } else {
                            printf("Error: Expected closing parenthesis ')'\n");
                        }
                    } else {
                        printf("Error: Expected integer literal\n");
                    }
                } else {
                    printf("Error: Expected relational operator\n");
                }
            } else {
                printf("Error: Expected variable\n");
            }
        } else {
            printf("Error: Expected opening parenthesis '('\n");
        }
    }

    // do_while_statement Check for the KEYWORD token (must be "exec")
    else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "exec") == 0) {
        printf("(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
        (*index)++;

        // Check for opening curly braces 'exec {'
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening curly braces
            (*index)++;

            // Check for the KEYWORD token (must be "disp") 'exec { disp'
            if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                (*index)++;

                // Check for opening parenthesis  'exec { disp ('
                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                    (*index)++;

                    // Handle STRING_LITERAL token 'exec { disp (string_literal'
                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                        printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                        (*index)++;

                        // Check for closing parenthesis 'exec { disp (string_literal) //'
                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                printf("(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                (*index)++;

                            // Check for 'exec { disp (string_literal) //'
                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                (*index)++;

                                // Check for closing curly braces 'exec { disp (string_literal) // variable'
                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                    (*index)++;

                                    // Check for assignment operator 'exec { disp (string_literal) // variable +'
                                    if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                        printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                        (*index)++;

                                        // Check for assignment operator 'exec { disp (string_literal) // variable ++'
                                        if (strcmp(tokens[*index].tokenType, "ARITHMETIC_OP") == 0) {
                                            printf("(ARITHMETIC_OP: %s)", tokens[*index].lexeme); // Print arithmetic operator
                                            (*index)++;

                                            // Check for opening curly braces 'exec { disp (string_literal) // variable ++ //'
                                            if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print line terminator
                                                (*index)++;

                                                // Check for closing curly braces 'exec { disp (string_literal) // variable ++ // }'
                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                    printf("(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                                    (*index)++;

                                                    // Check for keyword 'dum'
                                                    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "dum") == 0) {
                                                        printf("(ITERATIVE_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print dum
                                                        (*index)++;

                                                        // Check for opening parenthesis 'dum ('
                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                            (*index)++;

                                                            // Check for variable 'dum (variable'
                                                            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                                printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                                (*index)++;

                                                                // Check for relational operator 'dum (variable relational_op'
                                                                if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                                                    printf("(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print relational operator
                                                                    (*index)++;

                                                                    // Check for INTEGER_LITERAL token 'dum (variable relational_op INTEGER_LITERAL'
                                                                    if (strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0) {
                                                                        printf("(INTEGER_LITERAL: %s)", tokens[*index].lexeme); // Print INTEGER_LITERAL
                                                                        (*index)++;

                                                                        // Check for closing parenthesis 'dum (variable relational_op INTEGER_LITERAL)'
                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                            (*index)++;

                                                                            // Check for opening curly braces 'dum (variable relational_op INTEGER_LITERAL) //'
                                                                            if (strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                                printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print line terminator
                                                                                (*index)++;
                                                                            } else {
                                                                                printf("Error: Expected opening curly braces '{'\n");
                                                                            }
                                                                        } else {
                                                                            printf("Error: Expected closing parenthesis ')'\n");
                                                                        }
                                                                    } else {
                                                                        printf("Error: Expected integer literal\n");
                                                                    }
                                                                } else {
                                                                    printf("Error: Expected relational operator\n");
                                                                }
                                                            } else {
                                                                printf("Error: Expected variable\n");
                                                            }
                                                        } else {
                                                            printf("Error: Expected opening parenthesis '('\n");
                                                        }
                                                    } else {
                                                        printf("Error: Expected keyword 'dum'\n");
                                                    }
                                                } else {
                                                    printf("Error: Expected closing curly braces '}'\n");
                                                }
                                            } else {
                                                printf("Error: Expected line terminator (//)\n");
                                            }
                                        } else {
                                            printf("Error: Expected arithmetic operator\n");
                                        }
                                    } else {
                                        printf("Error: Expected arithmetic operator\n");
                                    }
                                } else {
                                    printf("Error: Expected variable\n");
                                }
                            } else {
                                printf("Error: Expected line terminator (//)\n");
                            }
                        } else {
                            printf("Error: Expected closing parenthesis ')'\n");
                        }
                    } else {
                        printf("Error: Expected string literal\n");
                    }
                } else {
                    printf("Error: Expected opening parenthesis '('\n");
                }
            } else {
                printf("Error: Expected output keyword 'disp'\n");
            }
        } else {
            printf("Error: Expected opening curly braces '{'\n");
        }
    } else {
        printf("Error: Expected iterative keyword 'pro', 'dum', or 'exec'\n");
    }                                          
}
    
// Conditional Statement parser
void Conditional_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;

    // Check for the INPUT_KEYWORD token (e.g., 'quando')
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "quando") == 0) {
        printf("(CONDITIONAL_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
        (*index)++;

        // Check for opening parenthesis 'quando ('
        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
            (*index)++;

            // Expect a variable 'quando (variable'
            if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                (*index)++;

                // Expect Relational OP 'quando (variable relational_op'
                if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                    printf("(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print variable
                    (*index)++;

                    // Check for init value 'quando (variable relational_op init_value'
                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                        strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                        strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                        strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0 ) {
                        printf("(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print init_value
                        (*index)++;

                        // Check for closing parenthesis ')' 'quando (variable relational_op init_value)'
                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                            (*index)++;
                                
                            // Check for opening curly braces 'quando (variable relational_op init_value) {' 
                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                (*index)++;

                                // Check for the KEYWORD token (must be "disp") 'quando (variable relational_op init_value) { disp'
                                if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                    printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                    (*index)++;

                                    // Check for opening parenthesis  'quando (variable relational_op init_value) { disp ('
                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                        (*index)++;

                                        // Handle STRING_LITERAL token 'quando (variable relational_op init_value) { disp (string_literal'
                                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                            printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                            (*index)++;

                                            // Check for closing parenthesis 'quando (variable relational_op init_value) { disp (string_literal)'
                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                (*index)++;
                                                        
                                                // Check for LINE_TERMINATOR token 'quando (variable relational_op init_value) { disp (string_literal) //'
                                                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                    printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                    (*index)++;

                                                   // Check for closing curly braces 'quando (variable relational_op init_value) { disp (string_literal) // }'
                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                        printf("(DELIMETER: %s))\n", tokens[*index].lexeme); // Print closing curly braces
                                                        (*index)++;

                                                        // Check for the KEYWORD token (must be "opt")  'opt    
                                                        if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "opt") == 0) {
                                                            printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                                            (*index)++;

                                                            // Check for opening parenthesis 'opt ('
                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                                (*index)++;

                                                                // Expect a variable 'opt (variable'
                                                                if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
                                                                    printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print variable
                                                                    (*index)++;
                                                                            
                                                                    // Expect Relational OP 'opt (variable relational_op'
                                                                    if (strcmp(tokens[*index].tokenType, "RELATIONAL_OP") == 0) {
                                                                        printf("(RELATIONAL_OP: %s)", tokens[*index].lexeme); // Print variable
                                                                        (*index)++;

                                                                        // Check for init value 'opt (variable relational_op init_value'
                                                                         if (*index < tokenCount && strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                                                                            strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                                                                            strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                                                                            strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0 ) {
                                                                            printf("(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print init_value
                                                                            (*index)++;

                                                                            // Check for closing parenthesis 'opt (variable relational_op init_value)'
                                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                (*index)++;

                                                                                // Check for opening curly braces 'opt (variable relational_op init_value) {'
                                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                    (*index)++;

                                                                                    // Check for 'opt (variable relational_op init_value) { disp'
                                                                                    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                                                                        printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                                                                        (*index)++;

                                                                                        // Check for 'opt (variable relational_op init_value) { disp('
                                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                                                            (*index)++;

                                                                                            // Check for 'opt (variable relational_op init_value) { disp(STRING_LITERAL'
                                                                                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                                                                                printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                                                                                (*index)++;

                                                                                                // Check for 'opt (variable relational_op init_value) { disp(STRING_LITERAL)'
                                                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                                    (*index)++;     

                                                                                                    // Check for 'opt (variable relational_op init_value) { disp(STRING_LITERAL) //'
                                                                                                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                                                                        printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                                                                        (*index)++;
                                                                                                                            
                                                                                                        // Check for 'opt (variable relational_op init_value) { disp(STRING_LITERAL) // }'
                                                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing curly braces
                                                                                                            (*index)++;

                                                                                                            // Check for 'alt'
                                                                                                            if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "alt") == 0) {
                                                                                                                printf("(CONDITIONAL_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
                                                                                                                (*index)++;

                                                                                                                // Check for 'alt {'
                                                                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                                                    (*index)++;

                                                                                                                    // Check for 'alt { disp'
                                                                                                                    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                                                                                                        printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                                                                                                        (*index)++;

                                                                                                                        // Check for 'alt { disp('
                                                                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                                                                                            (*index)++;

                                                                                                                            // Check for 'alt { disp(STRING_LITERAL'
                                                                                                                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                                                                                                                printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                                                                                                                (*index)++;

                                                                                                                                // Check for 'alt { disp(STRING_LITERAL)'
                                                                                                                                if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                                                                                    printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                                                                    (*index)++;  

                                                                                                                                    // Check for 'alt { disp(STRING_LITERAL) //'
                                                                                                                                    if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                                                                                                        printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                                                                                                        (*index)++;

                                                                                                                                        // Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                                                                        if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                                                                                            printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing curly braces
                                                                                                                                            (*index)++;
                                                                                                                                        } else { // Error Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                                                                            printf("Error: Expected closing curly braces '}'\n");
                                                                                                                                        }
                                                                                                                                    } else { // Error Check for 'alt { disp(STRING_LITERAL) //'
                                                                                                                                        printf("Error: Expected line terminator (//)\n");
                                                                                                                                    }
                                                                                                                                } else { // Error Check for 'alt { disp(STRING_LITERAL)'
                                                                                                                                    printf("Error: Expected closing parenthesis ')'\n");
                                                                                                                                }
                                                                                                                            } else { // Error Check for 'alt { disp( STRING_LITERAL'
                                                                                                                                printf("Error: Expected string literal\n");
                                                                                                                            }
                                                                                                                        } else { // Error Check for 'alt { disp('
                                                                                                                            printf("Error: Expected opening parenthesis '('\n");
                                                                                                                        }
                                                                                                                    } else { // Error Check for 'alt { disp'
                                                                                                                        printf("Error: Expected output keyword 'disp'\n");
                                                                                                                    }                                                        
                                                                                                                } else { // Error Check for 'alt {'
                                                                                                                    printf("Error: Expected opening curly braces '{'\n");
                                                                                                                }
                                                                                                            } else { // Error Check for 'alt'
                                                                                                                printf("Error: Expected conditional keyword 'alt'\n");
                                                                                                            }
                                                                                                        } else { // Error Check for 'opt (variable relational_op init_value) { disp (string_literal) // }'
                                                                                                            printf("Error: Expected closing curly braces '}'\n");
                                                                                                        }
                                                                                                    } else { // Error Check for 'opt (variable relational_op init_value) { disp (string_literal) //'
                                                                                                        printf("Error: Expected line terminator (//)\n");
                                                                                                    }
                                                                                                } else { // Error Check for 'opt (variable relational_op init_value) { disp (string_literal)'
                                                                                                    printf("Error: Expected closing parenthesis ')'\n");
                                                                                                }
                                                                                            } else { // Error Check for 'opt (variable relational_op init_value) { disp(string_literal'
                                                                                                printf("Error: Expected string literal\n");
                                                                                            }
                                                                                        } else { // Error Check for 'opt (variable relational_op init_value) { disp('
                                                                                            printf("Error: Expected opening parenthesis '('\n");
                                                                                        }
                                                                                    } else { // Error Check for 'opt (variable relational_op init_value) { disp'
                                                                                        printf("Error: Expected output keyword 'disp'\n");
                                                                                    }
                                                                                } else { // Error Check for 'opt (variable relational_op init_value) {'
                                                                                    printf("Error: Expected opening curly braces '{'\n");
                                                                                }
                                                                            } else { // Error Check for 'opt (variable relational_op init_value)'
                                                                                printf("Error: Expected closing parenthesis\n");
                                                                            }
                                                                        } else { // Error Check for 'opt (variable relational_op init_value'
                                                                            printf("Error: Expected init_value\n");
                                                                        }
                                                                    } else { // Error Check for 'opt (variable relational_op'
                                                                        printf("Error: Expected relational_op\n");
                                                                    }
                                                                } else { // Error Check for 'opt (variable'
                                                                    printf("Error: Expected variable\n");
                                                                }
                                                            } else { // Error Check for 'opt ('
                                                                printf("Error: Expected opening parenthesis '('\n");
                                                            }

                                                        // If no opt encountered check for 'alt'
                                                        } else if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "alt") == 0) { 
                                                            printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print CONDITIONAL_KEYWORD
                                                            (*index)++;

                                                            // Check for opening curly braces 'alt {'
                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "{") == 0) {
                                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                (*index)++;

                                                                // Check for 'alt { disp'
                                                                if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                                                    printf("(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                                                    (*index)++;

                                                                    // Check for 'alt { disp('
                                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "(") == 0) {
                                                                        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print opening parenthesis
                                                                        (*index)++;

                                                                        // Check for 'alt { disp(STRING_LITERAL'
                                                                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0) {
                                                                            printf("(STRING_LITERAL: %s)", tokens[*index].lexeme); // Print STRING_LITERAL
                                                                            (*index)++;

                                                                            // Check for 'alt { disp(STRING_LITERAL)'
                                                                            if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, ")") == 0) {
                                                                                printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing parenthesis
                                                                                (*index)++;  

                                                                                // Check for 'alt { disp(STRING_LITERAL) //'
                                                                                if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                                                                    printf("(LINE_TERMINATOR: %s)", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                                                                    (*index)++;

                                                                                    // Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                    if (strcmp(tokens[*index].tokenType, "DELIMETER") == 0 && strcmp(tokens[*index].lexeme, "}") == 0) {
                                                                                        printf("(DELIMETER: %s)", tokens[*index].lexeme); // Print closing curly braces
                                                                                        (*index)++;
                                                                                    } else { // Error Check for 'alt { disp(STRING_LITERAL) // }'
                                                                                        printf("Error: Expected closing curly braces '}'\n");
                                                                                    }
                                                                                } else { // Error Check for 'alt { disp(STRING_LITERAL) //'
                                                                                    printf("Error: Expected line terminator (//)\n");
                                                                                }
                                                                            } else { // Error Check for 'alt { disp(STRING_LITERAL)'    
                                                                                printf("Error: Expected closing parenthesis ')'\n");
                                                                            }
                                                                        } else { // Error Check for 'alt { disp(STRING_LITERAL'
                                                                            printf("Error: Expected string literal\n");
                                                                        }
                                                                    } else { // Error Check for 'alt { disp('
                                                                        printf("Error: Expected opening parenthesis '('\n");
                                                                    }
                                                                } else { // Error Check for 'alt { disp'
                                                                    printf("Error: Expected output keyword 'disp'\n");
                                                                }
                                                            } else { // Error Check for 'alt {'
                                                                printf("Error: Expected opening curly braces '{'\n");
                                                            }
                                                        } else { // Error Check for 'opt' or 'alt
                                                            printf("Error: Expected output keyword 'opt' or 'alt' \n");
                                                        }
                                                    } else { // Error Check for 'quando (variable relational_op init_value) { disp (string_literal) // }'
                                                        printf("Error: Expected closing curly braces ')'\n");
                                                    }
                                                } else { // Error Check for 'quando (variable relational_op init_value) { disp (string_literal) //'
                                                    printf("Error: Expected line terminator (//)\n");
                                                }        
                                            } else { // Error Check for 'quando (variable relational_op init_value) { disp (string_literal)'
                                                printf("Error: Expected closing parenthesis ')'\n");
                                            }
                                        } else { // Error Check for 'quando (variable relational_op init_value) { disp (string_literal'
                                            printf("Error: Expected string literal\n");
                                        }        
                                    } else { // Error Check for  'quando (variable relational_op init_value) { disp ('
                                        printf("Error: Expected opening parenthesis '('\n");
                                    }
                                } else { // Error Check for 'quando (variable relational_op init_value) { disp'
                                    printf("Error: Expected output keyword 'disp'\n");
                                }
                            } else { // Error Check for 'quando (variable relational_op init_value) {'
                                printf("Error: Expected opening curly braces '{'\n");
                            }
                        } else { // Error Check for 'quando (variable relational_op init_value)'
                            printf("Error: Expected closing parenthesis ')'\n");
                        }
                    } else { // Error Check for 'quando (variable relational_op init_value'
                        printf("Error: Expected init_value\n");
                    }
                } else { // Error Check for 'quando (variable relational_op'
                    printf("Error: Expected relational operator\n");
                }
            } else { // Error Check for 'quando (variable'
                    printf("Error: Expected variable\n");
            }
        } else { // Error check for opening parenthesis 'quando ('
            printf("Error: Expected opening parenthesis '('\n");
        }
    } else { // Error check for the INPUT_KEYWORD token (e.g., 'quando')
        printf("Error: Expected conditional keyword 'quando'\n");
    }
}

// Declaration Statement parser
void Declaration_Stmt(Token* tokens, int* index, int tokenCount) {
    if (*index >= tokenCount) return;   

    // Check for KEYWORD token (data type)
    if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && 
        strcmp(tokens[*index].lexeme, "numbra") == 0 ||
        strcmp(tokens[*index].lexeme, "deca") == 0 || 
        strcmp(tokens[*index].lexeme, "duplus") == 0 || 
        strcmp(tokens[*index].lexeme, "signa") == 0 ||
        strcmp(tokens[*index].lexeme, "binar") == 0) {
        printf("(DECLARATION_STATEMENT(KEYWORD(Data Type): %s)", tokens[*index].lexeme); // Print KEYWORD (data type)
        (*index)++;

        // Check for ( data_type VARIABLE
        if (strcmp(tokens[*index].tokenType, "VARIABLE") == 0) {
            printf("(VARIABLE: %s)", tokens[*index].lexeme); // Print VARIABLE
            (*index)++;

            // Check for ( data_type VARIABLE // )
            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                (*index)++;

                // Check for ( data_type VARIABLE, VARIABLE, VARIABLE // )
                } else if (processCommaAndExpectVariable(tokens, index, tokenCount)) {
                        
                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                            printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                            (*index)++;
                        } else {
                            printf("Error: Expected line terminator (//)\n");
                        }
                    // Check for ( data_type VARIABLE =   
                    } else if (*index < tokenCount && strcmp(tokens[*index].tokenType, "ASSIGNMENT_OP") == 0) {
                        printf("(ASSIGNMENT_OP: %s)", tokens[*index].lexeme);
                        (*index)++;

                        // Check for ( data_type VARIABLE =  init_value
                        if (*index < tokenCount && strcmp(tokens[*index].tokenType, "INTEGER_LITERAL") == 0 ||
                            strcmp(tokens[*index].tokenType, "FLOAT_LITERAL") == 0 || 
                            strcmp(tokens[*index].tokenType, "STRING_LITERAL") == 0 || 
                            strcmp(tokens[*index].tokenType, "CHAR_LITERAL") == 0 ) {
                            printf("(%s: %s)", tokens[*index].tokenType, tokens[*index].lexeme); // Print init_value
                            (*index)++;

                            // Check for ( data_type VARIABLE =  init_value // )
                            if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                                printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                                (*index)++;
                                } else {
                                printf("Error: Expected line terminator (//)\n");
                            }

                                // Check for the KEYWORD token (must be "disp")
                                if (strcmp(tokens[*index].tokenType, "KEYWORD") == 0 && strcmp(tokens[*index].lexeme, "disp") == 0) {
                                    printf("(OUTPUT_STATEMENT(KEYWORD: %s)", tokens[*index].lexeme); // Print KEYWORD
                                    (*index)++;

                        } else {
                            printf("Error: Expected init_value\n");
                        }
            // Check for datatype variable //
            } else if (*index < tokenCount && strcmp(tokens[*index].tokenType, "LINE_TERMINATOR") == 0) {
                printf("(LINE_TERMINATOR: %s))\n", tokens[*index].lexeme); // Print LINE_TERMINATOR
                (*index)++;
            } else {
                printf("Error: Expected line terminator (//)\n");
            }
        } else {\
            printf("Error: Expected variable\n");
        }
    } else {
        printf("Error: Expected data type keyword\n");
    }

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
        } else if (strcmp(tokens[index].tokenType, "KEYWORD") == 0 && strcmp(tokens[index].lexeme, "quando") == 0) {
            // "disp" keyword indicates the start of an output statement
            Conditional_Stmt(tokens, &index, tokenCount);
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
