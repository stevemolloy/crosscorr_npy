CC = clang
CFLAGS = -Wall -Wextra -Wpedantic -Werror -Wno-unused-function -ggdb -O3
CLIBS = -lm

SRC = src
OBJ = obj
BIN = bin

EXE = $(BIN)/cross_corr

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

all: $(EXE)

$(EXE): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CLIBS) $^ -o $@

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN):
	@mkdir -p $(BIN)

$(OBJ):
	@mkdir -p $(OBJ)

clean:
	rm -rf bin obj
