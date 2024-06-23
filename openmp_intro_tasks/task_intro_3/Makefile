CC   = gcc
OPTS = -fopenmp
SRC  = main.c
BIN  = isolate.elf

all: build

build:
	$(CC) $(SRC) $(OPTS) -o $(BIN)

clean:
	rm -rf $(BIN)
