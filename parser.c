#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100
#define MAX_STACK_SIZE 100

typedef struct {
    char tokenType[MAX_TOKEN_LENGTH];
    char lexeme[MAX_TOKEN_LENGTH];
    int line;
} Token;

typedef struct {
    Token stack[MAX_STACK_SIZE];
    int top;
} Stack;

// Stack operations
void initStack(Stack* s) {
    s->top = -1;  // Stack is empty initially
}

int isEmpty(Stack* s) {
    return s->top == -1;
}

int isFull(Stack* s) {
    return s->top == MAX_STACK_SIZE - 1;
}

void push(Stack* s, Token token) {
    if (isFull(s)) {
        printf("Stack overflow\n");
        return;
    }
    s->stack[++(s->top)] = token;
}

Token pop(Stack* s) {
    if (isEmpty(s)) {
        printf("Stack underflow\n");
        Token emptyToken = {"", "", -1};  // Return an empty token
        return emptyToken;
    }
    return s->stack[(s->top)--];
}

Token peek(Stack* s) {
    if (isEmpty(s)) {
        Token emptyToken = {"", "", -1};  // Return an empty token
        return emptyToken;
    }
    return s->stack[s->top];
}

Token currentToken; // Global token for current processing

// Function to get the next token from the tokens array
void nextToken(Token* tokens, int* index, Stack* stack) {
    currentToken = tokens[*index];
    push(stack, currentToken);
    (*index)++;
}

// Function to parse <output_stmt>
int parse_output_stmt(Token* tokens, int count, int* index, Stack* stack) {
    if (strcmp(currentToken.tokenType, "FUNCTION") == 0 && strcmp(currentToken.lexeme, "disp") == 0) {
        nextToken(tokens, index, stack); // Consume 'disp'

        if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, "(") == 0) {
            nextToken(tokens, index, stack); // Consume '('

            if (strcmp(currentToken.tokenType, "STRING_LITERAL") == 0) {
                nextToken(tokens, index, stack); // Consume STRING_LITERAL

                // Optional: Check for ',' VARIABLE or '@' VARIABLE
                if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, ",") == 0) {
                    nextToken(tokens, index, stack); // Consume ','

                    if (strcmp(currentToken.tokenType, "VARIABLE") == 0 ||
                        (strcmp(currentToken.lexeme, "@") == 0 && tokens[*index].tokenType == "VARIABLE")) {
                        if (strcmp(currentToken.lexeme, "@") == 0) {
                            nextToken(tokens, index, stack); // Consume '@'
                        }
                        nextToken(tokens, index, stack); // Consume VARIABLE

                        if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, ")") == 0) {
                            nextToken(tokens, index, stack); // Consume ')'

                            if (strcmp(currentToken.tokenType, "LINE_TERMINATOR") == 0) {
                                nextToken(tokens, index, stack); // Consume '//'
                                printf("Valid <output_stmt>\n");
                                return 1; // Successfully parsed output statement
                            }
                        }
                    }
                } else if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, ")") == 0) {
                    // Handle the case where no variable follows the string
                    nextToken(tokens, index, stack); // Consume ')'

                    if (strcmp(currentToken.tokenType, "LINE_TERMINATOR") == 0) {
                        nextToken(tokens, index, stack); // Consume '//'
                        printf("Valid <output_stmt>\n");
                        return 1; // Successfully parsed output statement
                    }
                }
            }
        }
    }
    printf("Error: Invalid <output_stmt>\n");
    return 0; // Failed to parse output statement
}

// Function to parse <input_stmt> rule using stack
int parse_input_stmt(Token* tokens, int count, int* index, Stack* stack) {
    if (strcmp(currentToken.tokenType, "FUNCTION") == 0 && strcmp(currentToken.lexeme, "fetch") == 0) {
        nextToken(tokens, index, stack); // Consume 'fetch'

        if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, "(") == 0) {
            nextToken(tokens, index, stack); // Consume '('

            if (strcmp(currentToken.tokenType, "STRING_LITERAL") == 0) {
                nextToken(tokens, index, stack); // Consume "<format_specifier>"

                if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, ",") == 0) {
                    nextToken(tokens, index, stack); // Consume ','

                    if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, "&") == 0) {
                        nextToken(tokens, index, stack); // Consume '&'

                        if (strcmp(currentToken.tokenType, "VARIABLE") == 0) {
                            nextToken(tokens, index, stack); // Consume 'VARIABLE'

                            if (strcmp(currentToken.tokenType, "DELIMETER") == 0 && strcmp(currentToken.lexeme, ")") == 0) {
                                nextToken(tokens, index, stack); // Consume ')'

                                if (strcmp(currentToken.tokenType, "LINE_TERMINATOR") == 0) {
                                    nextToken(tokens, index, stack); // Consume '//'
                                    printf("Valid <input_stmt>\n");
                                    return 1; // Successfully parsed input statement
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    printf("Error: Invalid <input_stmt>\n");
    return 0; // Failed to parse input statement
}

// Function to validate <parameter_list> rule using stack
int validateParameterList(Token* tokens, int count, int* index, Stack* stack) {
    int i = *index;

    // Rule: "void"
    if (i < count && strcmp(tokens[i].tokenType, "KEYWORD") == 0 && strcmp(tokens[i].lexeme, "void") == 0) {
        *index = i + 1;
        return 1;
    }

    // Rule: <parameter>
    if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
        i++;
        // Check if it's <parameter> "," <parameter_list>
        if (i < count && strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, ",") == 0) {
            i++;
            int subRuleValid = validateParameterList(tokens, count, &i, stack);
            if (subRuleValid) {
                *index = i;
                return 1;
            }
        } else {
            *index = i;
            return 1; // Only <parameter>
        }
    }

    return 0; // Invalid
}

// Function to validate <declaration_stmt> rule using stack
int validateDeclarationStmt(Token* tokens, int count, int* index, Stack* stack) {
    int i = *index;

    // Check for <data_type>
    if (i < count &&
        strcmp(tokens[i].tokenType, "KEYWORD") == 0 &&
        (strcmp(tokens[i].lexeme, "numbra") == 0 ||
         strcmp(tokens[i].lexeme, "deca") == 0 ||
         strcmp(tokens[i].lexeme, "duplos") == 0 ||
         strcmp(tokens[i].lexeme, "signa") == 0 ||
         strcmp(tokens[i].lexeme, "binar") == 0)) {
        i++; // Move to <var_list>

        // Check for <var_list>
        if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
            i++;

            // Handle optional "," <var_list>
            while (i < count && strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, ",") == 0) {
                i++;
                if (i < count && strcmp(tokens[i].tokenType, "VARIABLE") == 0) {
                    i++;
                } else {
                    return 0; // Invalid if no VARIABLE follows the ","
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

// Function to validate main_function rule using stack
int validateMainFunction(Token* tokens, int count, Stack* stack) {
    int i = 0;

    if (count >= 7 &&
        strcmp(tokens[i].tokenType, "KEYWORD") == 0 && strcmp(tokens[i].lexeme, "starto") == 0 &&
        strcmp(tokens[i + 1].tokenType, "DELIMETER") == 0 && strcmp(tokens[i + 1].lexeme, "(") == 0) {
        i += 2; // Move to inside the parentheses

        // Check for optional <parameter_list>
        if (validateParameterList(tokens, count, &i, stack) || 
            (i < count && strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, ")") == 0)) {
            if (strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, ")") == 0 &&
                strcmp(tokens[i + 1].tokenType, "DELIMETER") == 0 && strcmp(tokens[i + 1].lexeme, "{") == 0) {
                i += 2;

                // Check for optional <declaration_stmt>
                int declarationValid = validateDeclarationStmt(tokens, count, &i, stack);

                // Check for closing }
                if (i < count &&
                    strcmp(tokens[i].tokenType, "DELIMETER") == 0 && strcmp(tokens[i].lexeme, "}") == 0 &&
                    strcmp(tokens[i + 1].tokenType, "KEYWORD") == 0 && strcmp(tokens[i + 1].lexeme, "endo") == 0) {
                    return 1; // Valid main_function
                }
            }
        }
    }

    return 0; // Invalid main_function
}

// Function to check if the token matches any rule
const char* matchGrammarRule(Token* tokens, int count) {
    Stack stack;
    initStack(&stack);

    if (validateMainFunction(tokens, count, &stack)) {
        return "<main_function> ::= <data_type> \"starto\" \"(\" [ <parameter_list> ] \")\" <main_body> \"endo\"";
    }

    if (validateDeclarationStmt(tokens, count, &(int){0}, &stack)) {
        return "<declaration_stmt> ::= <data_type> <var_list> LINE_TERMINATOR";
    }

    if (parse_input_stmt(tokens, count, &(int){0}, &stack)) {
        return "<input_stmt> ::= \"fetch\" \"(\" \"<format_specifier>\" \",\" \"&\" VARIABLE \")\" LINE_TERMINATOR";
    }

    if (parse_output_stmt(tokens, count, &(int){0}, &stack)) {
        return "<output_stmt> ::= disp(\"Your age is %d.\" , @age)LINE_TERMINATOR";
    }

    return "Input does not match any grammar rule.";
}

// Function to read tokens from the file
int readTokens(FILE* file, Token* tokens, int maxTokens) {
    int count = 0;
    while (count < maxTokens && fscanf(file, "%s %s %d", tokens[count].tokenType, tokens[count].lexeme, &tokens[count].line) != EOF) {
        count++;
    }
    return count;
}

int main() {
    FILE* file = fopen("symboltable1.leon", "r");
    if (!file) {
        printf("Error: Could not open symboltable1.leon\n");
        return 1;
    }

    Token tokens[100];
    int tokenCount = readTokens(file, tokens, 100);
    fclose(file);

    const char* result = matchGrammarRule(tokens, tokenCount);
    printf("Result: %s\n", result);

    return 0;
}