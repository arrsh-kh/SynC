#include <stdio.h>
#include <string.h>
#include "codegen.h"

static void emit_expr(Expr* expr) {
    if (!expr) return;

    switch (expr->type) {
        case EXPR_NUMBER:
            printf("%d", expr->number);
            break;
        case EXPR_VAR:
            printf("%s", expr->var);
            break;
        case EXPR_BINARY:
            printf("(");
            emit_expr(expr->binary.left);
            if (expr->binary.op == 'F') {
                printf(" / ");  // Treat floor division same as integer division in C
            } else {
                printf(" %c ", expr->binary.op);
            }
            emit_expr(expr->binary.right);
            printf(")");
            break;
        case EXPR_CALL:
            if (strcmp(expr->call.name, "print") == 0) {
                printf("printf(\"%%d\\n\", ");
                for (int i = 0; i < expr->call.argc; i++) {
                    emit_expr(expr->call.args[i]);
                    if (i < expr->call.argc - 1) printf(", ");
                }
                printf(")");
            } else {
                printf("%s(", expr->call.name);
                for (int i = 0; i < expr->call.argc; i++) {
                    emit_expr(expr->call.args[i]);
                    if (i < expr->call.argc - 1) printf(", ");
                }
                printf(")");
            }
            break;
    }
}

static void emit_stmt(Statement* stmt) {
    switch (stmt->type) {
        case STMT_ASSIGN:
            printf("    int %s = ", stmt->name);
            emit_expr(stmt->expr);
            printf(";\n");
            break;
        case STMT_RETURN:
            printf("    return ");
            emit_expr(stmt->expr);
            printf(";\n");
            break;
        case STMT_EXPR:
            printf("    ");
            emit_expr(stmt->expr);
            printf(";\n");
            break;
    }
}

void generate_c_code(Function* func) {
    printf("#include <stdio.h>\n\n");
    
    int has_return = 0;
    for (int i = 0; i < func->body_len; i++) {
        if (func->body[i]->type == STMT_RETURN) {
            has_return = 1;
            break;
        }
    }
    // Function declaration
    printf("%s %s() {\n", has_return ? "int" : "void", func->name);
    for (int i = 0; i < func->body_len; i++) {
        emit_stmt(func->body[i]);
    }
    printf("}\n\n");

    // main()
    printf("int main() {\n");
    if (func->body_len > 0 && func->body[func->body_len - 1]->type == STMT_RETURN) {
   printf("    printf(\"%%d\\n\", %s());\n", func->name);
    } else {
        printf("    %s();\n", func->name);
    }
    printf("    return 0;\n");
    printf("}\n");
}
