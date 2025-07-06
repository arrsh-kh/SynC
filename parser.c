#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token* tokens;
static int pos = 0;

static Token peek() {
    return tokens[pos];
}

static Token advance() {
    return tokens[pos++];
}

static int match(TokenType type) {
    if (peek().type == type) {
        advance();
        return 1;
    }
    return 0;
}

static void expect(TokenType type, const char* err) {
    if (!match(type)) {
        fprintf(stderr, "Parse Error: expected %s, got '%s'\n", err, peek().value);
        exit(1);
    }
}

// Error if any bracket is unbalanced
static void expect_bracket(TokenType type, const char* err) {
    if (peek().type != type) {
        fprintf(stderr, "Bracket Error: expected %s, got '%s'\n", err, peek().value);
        exit(1);
    }
    advance();
}

Expr* parse_expr();

Expr* parse_primary() {
    Token tok = advance();
    if (tok.type == TOKEN_NUMBER) {
        Expr* e = malloc(sizeof(Expr));
        e->type = EXPR_NUMBER;
        e->number = atoi(tok.value);
        return e;
    } else if (tok.type == TOKEN_IDENTIFIER) {
        if (peek().type == TOKEN_LPAREN) {
            // Call expression
            advance();  // (
            Expr** args = NULL;
            int argc = 0;

            while (peek().type != TOKEN_RPAREN) {
                Expr* arg = parse_expr();
                args = realloc(args, sizeof(Expr*) * (argc + 1));
                args[argc++] = arg;
                if (peek().type == TOKEN_RPAREN) break;
            }

            expect_bracket(TOKEN_RPAREN, ")");
            Expr* call = malloc(sizeof(Expr));
            call->type = EXPR_CALL;
            call->call.name = strdup(tok.value);
            call->call.args = args;
            call->call.argc = argc;
            return call;
        }

        // Variable reference
        Expr* e = malloc(sizeof(Expr));
        e->type = EXPR_VAR;
        e->var = strdup(tok.value);
        return e;
    }

    fprintf(stderr, "Parse Error: Unexpected token in expression: '%s'\n", tok.value);
    exit(1);
}

int get_precedence(TokenType type) {
    switch (type) {
        case TOKEN_STAR:
        case TOKEN_SLASH:
        case TOKEN_PERCENT:
        case TOKEN_DSLASH: return 2;
        case TOKEN_PLUS:
        case TOKEN_MINUS:  return 1;
        default: return -1;  // Return -1 for non-operators
    }
}

Expr* parse_expr_bp(int min_prec); // forward decl

Expr* parse_expr() {
    return parse_expr_bp(0);
}

Expr* parse_expr_bp(int min_prec) {
    Expr* left = parse_primary();

    while (1) {
        TokenType op_type = peek().type;
        int prec = get_precedence(op_type);
        if (prec < 0 || prec < min_prec) break;

        char op;
        switch (op_type) {
            case TOKEN_PLUS:    op = '+'; break;
            case TOKEN_MINUS:   op = '-'; break;
            case TOKEN_STAR:    op = '*'; break;
            case TOKEN_SLASH:   op = '/'; break;
            case TOKEN_PERCENT: op = '%'; break;
            case TOKEN_DSLASH:  op = 'F'; break; // Floor division
            default: return left;
        }

        advance(); // consume operator

        Expr* right = parse_expr_bp(prec + 1);

        Expr* bin = malloc(sizeof(Expr));
        bin->type = EXPR_BINARY;
        bin->binary.op = op;
        bin->binary.left = left;
        bin->binary.right = right;

        left = bin;
    }

    return left;
}

Statement* parse_stmt() {
    Token t = peek();

    if (t.type == TOKEN_RETURN) {
        advance();
        Expr* e = parse_expr();
        Statement* stmt = malloc(sizeof(Statement));
        stmt->type = STMT_RETURN;
        stmt->expr = e;
        stmt->name = NULL;
        return stmt;
    }

    if (t.type == TOKEN_IDENTIFIER) {
        Token next = tokens[pos + 1];

        if (next.type == TOKEN_EQUALS) {
            // Assignment
            char* name = strdup(t.value);
            advance();  // identifier
            advance();  // =
            Expr* value = parse_expr();
            Statement* stmt = malloc(sizeof(Statement));
            stmt->type = STMT_ASSIGN;
            stmt->name = name;
            stmt->expr = value;
            return stmt;
        } else {
            // Function call
            Expr* e = parse_expr();
            Statement* stmt = malloc(sizeof(Statement));
            stmt->type = STMT_EXPR;
            stmt->expr = e;
            stmt->name = NULL;
            return stmt;
        }
    }

    fprintf(stderr, "Parse Error: Unknown statement starting with '%s'\n", t.value);
    exit(1);
}

Function* parse_function() {
    expect(TOKEN_DEF, "def");
    Token name = advance();
    expect(TOKEN_LPAREN, "(");
    expect_bracket(TOKEN_RPAREN, ")");
    expect(TOKEN_LBRACE, "{");

    Function* fn = malloc(sizeof(Function));
    fn->name = strdup(name.value);
    fn->body = NULL;
    fn->body_len = 0;

    while (peek().type != TOKEN_RBRACE) {
        Statement* stmt = parse_stmt();
        fn->body = realloc(fn->body, sizeof(Statement*) * (fn->body_len + 1));
        fn->body[fn->body_len++] = stmt;
    }

    expect_bracket(TOKEN_RBRACE, "}");
    return fn;
}

// NEW: Parse multiple functions
Program* parse_program() {
    Program* prog = malloc(sizeof(Program));
    prog->functions = NULL;
    prog->func_count = 0;
    
    while (peek().type != TOKEN_EOF) {
        Function* func = parse_function();
        prog->functions = realloc(prog->functions, sizeof(Function*) * (prog->func_count + 1));
        prog->functions[prog->func_count++] = func;
    }
    
    return prog;
}

Program* parse(Token* toks) {
    tokens = toks;
    pos = 0;
    return parse_program();
}

// Recursive free for expressions
void free_expr(Expr* expr) {
    if (!expr) return;
    
    switch (expr->type) {
        case EXPR_VAR:
            free(expr->var);
            break;
        case EXPR_BINARY:
            free_expr(expr->binary.left);
            free_expr(expr->binary.right);
            break;
        case EXPR_CALL:
            free(expr->call.name);
            for (int i = 0; i < expr->call.argc; i++) {
                free_expr(expr->call.args[i]);
            }
            free(expr->call.args);
            break;
    }
    free(expr);
}

void free_ast(Program* prog) {
    for (int i = 0; i < prog->func_count; i++) {
        Function* func = prog->functions[i];
        free(func->name);
        for (int j = 0; j < func->body_len; j++) {
            Statement* s = func->body[j];
            free_expr(s->expr);
            if (s->name) free(s->name);
            free(s);
        }
        free(func->body);
        free(func);
    }
    free(prog->functions);
    free(prog);
}

// Free tokens
void free_tokens(Token* tokens) {
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        if (tokens[i].value) {
            free(tokens[i].value);
        }
    }
    free(tokens);
}
