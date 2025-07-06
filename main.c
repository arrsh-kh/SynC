#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

void print_token(Token token);

char* read_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fprintf(stderr, "Out of memory\n");
        exit(1);
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file.sync>\n", argv[0]);
        return 1;
    }

    // Step 1: Load source code
    char* source = read_file(argv[1]);
    fprintf(stderr, "Source code:\n%s\n", source);  

    // Step 2: Lexing
    Token* tokens = tokenize(source);
    
    // DEBUG: Print all tokens
    fprintf(stderr, "\nTokens:\n");  
    for (int i = 0; tokens[i].type != TOKEN_EOF; i++) {
        fprintf(stderr, "%d: ", i);  
        print_token(tokens[i]);     
    }
    fprintf(stderr, "\n");  

    // Step 3: Parsing
    Program* prog = parse(tokens);

    // Step 4: Generate C code (this should stay as printf to go to stdout)
    generate_c_code(prog);

    // Step 5: Cleanup
    free_ast(prog);
    free_tokens(tokens);
    free(source);
    return 0;
}
