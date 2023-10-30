CC   = mpicc
OPTS = -O2 -lm
SRC  = main.c
BIN  = task_3.elf

all: build

build:
	$(CC) $(SRC) $(OPTS) -o $(BIN)

launch:
	mpirun -np 2 ./$(BIN) > out.txt

debug:
	$(CC) $(SRC) $(OPTS) -DDEBUG_ON -o $(BIN)

clean:
	rm -rf *.elf
