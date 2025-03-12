//CS435P02: Recursive Descent Parser (with Scanner) for Simple_PL1
//Cristopher Ahuatl
//Arthur Peterson-Vetch
//  Implement a Recursive Descent Parser based on the grammar for Simple_Pl1.
//  Indicate parsing errors in output.

#include <stdio.h>  //for c I/O
#include <stdlib.h> //for exit()
#include <string.h> //for strcmp
#include "scanner.h"

//unsigned numErrs = 0;
enum tokenType currentToken;

/*========= PROVIDED FUNCTIONS ==========*/
void parse_error(char* errMsg, char* lexeme) {
    //extern unsigned numErrs; //for future if error recovery used
    //numErrs++;
    fprintf(stderr, "%s: %s\n", errMsg, lexeme);
}


void match(enum tokenType expected) {
    if (currentToken == expected) {
        currentToken = scan();
    }
    else {
        parse_error("Expected symbol", mnemonic[expected]);
        exit(1);
    }
}

/*========== GRAMMAR RULE FUNCTION DECLARATIONS ==========*/
void program(FILE*);

void stmt_list(FILE*);

void stmt(FILE*);

void expr_list(FILE*);

void expr_list_tail(FILE*);

void id_list(FILE*);

void id_list_tail(FILE*);

void expression(FILE*);

void term_tail(FILE*);

void term(FILE*);

void factor_tail(FILE*);

void factor(FILE*);

int main(int argc, char* argv[]) {
    extern FILE* src;
    //enum tokenType currentToken;
    if (argc > 1) {
        if (fopen_s(&src, argv[1], "r")) {
            fprintf(stderr, "Error opening source file %s", argv[1]);
            exit(1);
        }
    }

    //begin process by taking in the first token of file, which should be ID, READ, WRITE, or EOF token
    while ((currentToken = scan()) != SCAN_EOF) {
        program(src);
    }
    printf("\nParsing Complete. No errors.\n");
    fclose(src);
    return 0;
}

/*=========== FUNCTION DEFINITIONS ==========*/
void program(FILE* src) {
    stmt_list(src);
    //eof symbol = $ in grammar
    if (currentToken != SCAN_EOF) {
        printf("Expected end of input");
    }

}

void stmt_list(FILE* src) {
    while (currentToken == ID || currentToken == READ || currentToken == WRITE) {
        //if next token starts a statement, call stmt()
        stmt(src);

        //multiple statements can occur in a file
        stmt_list(src);
    }
    //otherwise stop recursion

}

void stmt(FILE* src) {
    //a statement must start with and ID, READ, or WRITE
    if (currentToken == ID) {
        match(ID);
        match(ASSIGN);
        expression(src);
        match(SEMICOLON);
    }

    else if (currentToken == READ) {
        match(READ);
        match(LPAREN);

        //id list handles possibility of multiple IDs
        id_list(src);

        match(RPAREN);
        match(SEMICOLON);
    }
    else if (currentToken == WRITE) {
        match(WRITE);
        match(LPAREN);

        //expr_list handles the case of multiple expressions
        expr_list(src);

        match(RPAREN);
        match(SEMICOLON);
    }
    else {
        printf("invalid statement: %s\n", mnemonic[currentToken]);
    }

}

void expr_list(FILE* src) {
    expression(src);

    //expr_list_tail does nothing if no COMMA token encountered
    expr_list_tail(src);
}

void expr_list_tail(FILE* src) {
    if (currentToken == COMMA) {
        match(COMMA);
        expression(src);
        //call recursively until comma no longer encountered. Expect RPAREN when exiting
        expr_list_tail(src);
    }
}

void id_list(FILE* src) {
    match(ID);
    //ID_list_tail does nothing if no COMMA token encountered
    id_list_tail(src);
}

void id_list_tail(FILE* src) {
    if (currentToken == COMMA) {
        match(COMMA);
        match(ID);
        //if multiple IDs, recursively call until no more IDs. Function ends when other token encountered
        id_list_tail(src);
    }
}

void expression(FILE* src) {
    term(src);
    term_tail(src);
}

void term_tail(FILE* src) {
    if (currentToken == PLUS) {
        match(PLUS);
        term(src);
        term_tail(src);
    }
    else if (currentToken == MINUS) {
        match(MINUS);
        term(src);
        term_tail(src);
    }
}

void term(FILE* src) {
    factor(src);
    factor_tail(src);
}

void factor_tail(FILE* src) {
    if (currentToken == TIMES ) {
        auto temp = currentToken;
        match(TIMES);
        factor(src);
        factor_tail(src);
    }
    else if (currentToken == DIV) {
        match(DIV);
        
        if (currentToken == SEMICOLON || currentToken == RPAREN) {
            fprint("\nError in expression: Expected ID, NUMBER, or '(' . \n");
            exit(1);
        }

        factor(src);
        factor_tail(src);
    }
}

void factor(FILE* src) {
    if (currentToken == ID) {
        match(ID);
    }
    if (currentToken == NUMBER) {
        match(NUMBER);
    }
    else if (currentToken == LPAREN) {
        match(LPAREN);
        expression(src);
        match(RPAREN);
    }

}
