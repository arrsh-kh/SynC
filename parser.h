#pragma once

#include "lexer.h"

typedef enum {
    EXPR_NUMBER,
    EXPR_VAR,
    EXPR_BINARY,
    EXPR_CALL
} ExprType;

typedef struct Expr {
    ExprType type;
    union {
        int number;
        char* var;

        struct {
            struct Expr* left;
            char op;
            struct Expr* right;
        } binary;

        struct {
            char* name;
            struct Expr** args;
            int argc;
        } call;
    };
} Expr;

typedef enum {
    STMT_ASSIGN,
    STMT_RETURN,
    STMT_EXPR
} StmtType;

typedef struct {
    StmtType type;
    char* name;      // For assignment
    Expr* expr;
} Statement;

typedef struct {
    char* name;
    Statement** body;
    int body_len;
} Function;

// NEW: Program structure to hold multiple functions
typedef struct {
    Function** functions;
    int func_count;
} Program;

Program* parse(Token* tokens);
void free_ast(Program* prog);
void free_tokens(Token* tokens);
