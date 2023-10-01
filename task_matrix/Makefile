CC   = gcc
OPTS = -fopenmp
SRC  = main.c matrix.c
BIN  = matrix

BUILD_DIR   = build
RESULTS_DIR = results

DEFS = MATRIX_SIZE=512 N_THREADS=8
OPTS_DEFS = $(addprefix -D,$(DEFS))

all: build

build: build_default

$(BUILD_DIR)_dir:
	mkdir -p $(BUILD_DIR)

build_default: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DNOOPTIMIZE -o $(BUILD_DIR)/default_$(BIN)

run: run_default

run_default:
	./$(BUILD_DIR)/default_$(BIN)

clean:
	rm -rf ${BUILD_DIR} ${RESULTS_DIR}
