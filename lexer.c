#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"  // ← include your own header for consistency

#define INITIAL_CAPACITY 64

int is_symbol(char c) {
    return strchr("()+-*/%{}", c) != NULL;
}

TokenType symbol_type(const char** p) {
    switch (**p) {
        case '(': return TOKEN_LPAREN;
        case ')': return TOKEN_RPAREN;
        case '{': return TOKEN_LBRACE;
        case '}': return TOKEN_RBRACE;
        case '=': return TOKEN_EQUALS;
        case '+': return TOKEN_PLUS;
        case '-': return TOKEN_MINUS;
        case '*': return TOKEN_STAR;
        case '%': return TOKEN_PERCENT;
        case '/':
            if (*(*p + 1) == '/') {
                (*p)++;  // advance past second slash
                return TOKEN_DSLASH;
            }
            return TOKEN_SLASH;
        default: return TOKEN_UNKNOWN;
    }
}

Token make_token(TokenType type, const char* start, int length) {
    Token token;
    token.type = type;
    token.value = NULL;
    if (length > 0) {
        token.value = malloc(length + 1);
        strncpy(token.value, start, length);
        token.value[length] = '\0';
    }
    return token;
}

void print_token(Token token) {
    const char* names[] = {
    "DEF", "IDENTIFIER", "NUMBER",
    "(", ")", "{", "}",
    "=", "RETURN", "EOF", "UNKNOWN",
    "+", "-", "*", "/", "%", "//"
    };
    printf("Token: %-10s", names[token.type]);
    if (token.value) printf(" Value: %s", token.value);
    printf("\n");
}

Token* tokenize(const char* src) {
    const char* p = src;

    int capacity = INITIAL_CAPACITY;
    Token* tokens = malloc(sizeof(Token) * capacity);
    int count = 0;

    while (*p) {
        if (isspace(*p)) {
            p++; continue;
        }

        if (isalpha(*p) || *p == '_') {
            const char* start = p;
            while (isalnum(*p) || *p == '_') p++;
            int len = p - start;

            Token token;
            if (len == 3 && strncmp(start, "def", 3) == 0)
                token = make_token(TOKEN_DEF, start, len);
            else if (len == 6 && strncmp(start, "return", 6) == 0)
                token = make_token(TOKEN_RETURN, start, len);
            else
                token = make_token(TOKEN_IDENTIFIER, start, len);

            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, sizeof(Token) * capacity);
            }
            tokens[count++] = token;
        }

        else if (isdigit(*p)) {
            const char* start = p;
            while (isdigit(*p)) p++;
            Token token = make_token(TOKEN_NUMBER, start, p - start);

            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, sizeof(Token) * capacity);
            }
            tokens[count++] = token;
        }

        else if (is_symbol(*p)) {
            const char* start = p;
            TokenType type = symbol_type(&p);
            p++;  // move past first symbol (and second if `//`)
            Token token = make_token(type, start, p - start);

            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, sizeof(Token) * capacity);
            }
            tokens[count++] = token;
        }

        else {
            Token token = make_token(TOKEN_UNKNOWN, p, 1);
            p++;

            if (count >= capacity) {
                capacity *= 2;
                tokens = realloc(tokens, sizeof(Token) * capacity);
            }
            tokens[count++] = token;
        }
    }

    // Add EOF token
    tokens[count++] = make_token(TOKEN_EOF, NULL, 0);

    return tokens;
}
