#pragma once

#include <stdio.h>  //for c I/O
#include <stdlib.h> //for exit()
#include <ctype.h>  //for isalpha(), isalnum(), ...
#include <string.h> //for strcmp

//Added the tokenType and mnemonic SIZE_ERROR to return if a lexeme is too large to avoid overflow
enum tokenType {
    READ, WRITE, ID, NUMBER, LPAREN, RPAREN, SEMICOLON, COMMA, ASSIGN, PLUS, MINUS, TIMES, DIV, SCAN_EOF, SIZE_ERROR
};

char* mnemonic[] = { "READ", "WRITE", "ID", "NUMBER", "LPAREN", "RPAREN", "SEMICOLON", "COMMA", "ASSIGN", "PLUS", "MINUS", "TIMES", "DIV", "SCAN_EOF", "SIZE_ERROR" };

void lexical_error(char ch) {
    fprintf(stderr, "Lexical error. Unexpected character: %c  \n", ch);
}

char lexeme[256] = { '\0' };    //this means our valid range is 0 - 255
unsigned int lexLen = 0;
FILE* src;

enum tokenType scan() {
    static int currentCh = ' ';
    static int tempCh = ' ';
    char* reserved[2] = { "read", "write" };
    lexLen = 0;
    lexeme[0] = '\0';
    extern FILE* src;

    if (feof(src)) {    //EOF indicator is set when a read operation attempts to read past the end of a file
        return SCAN_EOF; // if end of file, return the eof mnemonic token
    }
    while ((currentCh = fgetc(src)) != EOF) { // while current scan doesn't hit end of file
        if (isspace(currentCh)) {    //skips whitespace characters when scanning and goes to next iteration
            continue;
        }
        /*===== IDENTIFIER LEXEME CASE =====*/

        else if (isalpha(currentCh) || currentCh == '_') {
            lexeme[0] = currentCh;
            lexLen = 1;   //at this stage, we currently have the first character of an identifier lexeme

            //use temp to peek at next character, evaluate if its valid for an identifier lexeme, get next char after
            for (tempCh = fgetc(src); isalnum(tempCh) || tempCh == '_'; tempCh = fgetc(src)) {
                if (lexLen < 255) { //256 reserved for '\0'
                    lexeme[lexLen] = tempCh;
                    lexLen++;
                }
                else {
                    fprintf(stderr, "Lexical error. Max lexeme length 255 reached\n");
                    //Scan the extra characters in the lexeme without storing them to
                    //avoid ignoring valid identifiers after an invalid size lexeme
                    for (tempCh = fgetc(src); isalnum(tempCh) || tempCh == '_'; tempCh = fgetc(src)) {
                        //this is empty because we just need to iterate through the lexeme
                    }
                }

            }
            lexeme[lexLen] = '\0'; //terminate lexeme by adding sentinel value
            ungetc(tempCh, src);   //return the first non-ID character back to the file stream

            //considering the regular expression definition for read/write is case sensitive, strcmp works
            if (strcmp(lexeme, reserved[0]) == 0) {
                return READ;
            }
            if (strcmp(lexeme, reserved[1]) == 0) {
                return WRITE;
            }
            if (lexLen >= 255) {
                return SIZE_ERROR;
            }

            return ID;
        }

        /*===== NUMBER LEXEME CASE =====*/
        else if (isdigit(currentCh)) {
            //BUILD LEXEME FOR THE NUMBER
            lexeme[0] = currentCh;
            lexLen = 1;
            for (tempCh = fgetc(src); isdigit(tempCh); tempCh = fgetc(src)) {

                if (lexLen < 255) { //256 reserved for '\0'
                    lexeme[lexLen] = tempCh;
                    lexLen++;
                }
                else {
                    fprintf(stderr, "Lexical error. Max lexeme length 255 reached\n");
                    //Scan the extra characters in the lexeme without storing them to
                    //avoid ignoring valid identifiers after an invalid size lexeme
                    for (tempCh = fgetc(src); isdigit(tempCh); tempCh = fgetc(src)) {
                        //empty
                    }
                }
            }
            //finish fixing lexeme string, ungetc the last character read that is not a digit and then return a NUMBER
            lexeme[lexLen] = '\0';
            ungetc(tempCh, src);

            if (lexLen >= 255) {
                return SIZE_ERROR;
            }
            return NUMBER;
        }

        /*===== OPERATOR TOKEN LEXEME CASE =====*/
        //use selection statements to look for tokens for operators and delimiters and the assignment (:=)

        else if (currentCh == ':') {
            tempCh = fgetc(src);
            if (tempCh == '=') {
                return ASSIGN;
            }
            ungetc(tempCh, src);    //unget the character before sending lexical error to avoid loss of character
            lexical_error(currentCh); //if lexeme is a semicolon but not followed by =, then return an error for the semicolon and preserve the following character
        }
        else if (currentCh == '+') {
            return PLUS;
        }
        else if (currentCh == '-') {
            return MINUS;
        }
        else if (currentCh == '*') {
            return TIMES;
        }
        else if (currentCh == '/') {
            return DIV;
        }
        else if (currentCh == ';') {
            return SEMICOLON;
        }
        else if (currentCh == ',') {
            return COMMA;
        }
        else if (currentCh == '(') {
            return LPAREN;
        }
        else if (currentCh == ')') {
            return RPAREN;
        }
        /*===== INVALID INPUT CHARACTER CASE =====*/
        else {
            lexical_error(currentCh);
        }
    }
    return SCAN_EOF;
}
