CC   = gcc
OPTS = -fopenmp
SRC  = main.c matrix.c
BIN  = matrix

all: build

build: build_seq

build_seq:
	$(CC) $(SRC) $(OPTS) -o seq_$(BIN)

run: run_seq

run_seq:
	./seq_$(BIN) > seq_$(BIN).res

clean: clean_seq

clean_seq:
	rm -rf seq_*
