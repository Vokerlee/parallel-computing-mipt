CC   = gcc
OPTS = -fopenmp -mavx -O1
SRC  = main.c matrix.c
BIN  = matrix

BUILD_DIR   = build
RESULTS_DIR = results

DEFS = MATRIX_SIZE=256 N_THREADS=1
OPTS_DEFS = $(addprefix -D,$(DEFS))

all: build

build: build_default build_opt1 build_opt2 build_opt3 build_opt4 build_strassen

$(BUILD_DIR)_dir:
	mkdir -p $(BUILD_DIR)

build_default: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DNOOPTIMIZE -o $(BUILD_DIR)/default_$(BIN)

build_opt1: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DOPT1_LOOP -o $(BUILD_DIR)/opt1_$(BIN)

build_opt2: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DOPT2 -o $(BUILD_DIR)/opt2_$(BIN)

build_opt3: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DOPT3 -o $(BUILD_DIR)/opt3_$(BIN)

build_opt4: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DOPT4 -o $(BUILD_DIR)/opt4_$(BIN)

build_strassen: $(BUILD_DIR)_dir
	$(CC) $(SRC) $(OPTS) $(OPTS_DEFS) -DOPT_STRASSEN -o $(BUILD_DIR)/opt_strassen_$(BIN)

run: run_default

run_default:
	./$(BUILD_DIR)/default_$(BIN)

run_opt1:
	./$(BUILD_DIR)/opt1_$(BIN)

run_opt2:
	./$(BUILD_DIR)/opt2_$(BIN)

run_opt3:
	./$(BUILD_DIR)/opt3_$(BIN)

run_opt4:
	./$(BUILD_DIR)/opt4_$(BIN)

run_strassen:
	./$(BUILD_DIR)/opt_strassen_$(BIN)

clean:
	rm -rf ${BUILD_DIR} ${RESULTS_DIR}
