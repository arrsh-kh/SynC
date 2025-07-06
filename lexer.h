#pragma once

typedef enum {
    TOKEN_DEF,
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_LPAREN, TOKEN_RPAREN,
    TOKEN_LBRACE, TOKEN_RBRACE,
    TOKEN_EQUALS,
    TOKEN_RETURN,
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_DSLASH,  // for //
} TokenType;

typedef struct {
    TokenType type;
    char* value;
} Token;

Token* tokenize(const char* src);
void print_token(Token token);  
