CC = gcc
SRC = main.c lexer.c parser.c codegen.c
BIN = syncc.out

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN) output.c run
