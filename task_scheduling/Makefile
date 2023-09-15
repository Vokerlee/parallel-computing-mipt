CC   = gcc
OPTS = -fopenmp
SRC  = main.c
BIN  = scheduling.elf

all: static

static: static_1 static_2

static_1:
	$(CC) $(SRC) $(OPTS) -DOMP_SCHEDLING_CLASS=static -DOMP_SCHEDLING_CHUNKS=1 -o $(BIN)_1

static_2:
	$(CC) $(SRC) $(OPTS) -DOMP_SCHEDLING_CLASS=static -DOMP_SCHEDLING_CHUNKS=2 -o $(BIN)_2

dynamic: dynamic_1 dynamic_2

dynamic_1:
	$(CC) $(SRC) $(OPTS) -DOMP_SCHEDLING_CLASS=dynamic -DOMP_SCHEDLING_CHUNKS=1 -o $(BIN)_1

dynamic_2:
	$(CC) $(SRC) $(OPTS) -DOMP_SCHEDLING_CLASS=dynamic -DOMP_SCHEDLING_CHUNKS=2 -o $(BIN)_2

guided:
	$(CC) $(SRC) $(OPTS) -DOMP_SCHEDLING_CLASS=guided -DOMP_SCHEDLING_CHUNKS=1 -o $(BIN)

clean:
	rm -rf $(BIN)*
