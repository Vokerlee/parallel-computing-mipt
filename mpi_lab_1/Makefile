x_size = 10
y_size = 10

all: build run_1 run_2 run_3 run_4

build:
	mkdir -p build
	mpicc main.c -O2 -lgmp -lm -o ./build/main.out

clean:
	rm -rf build 

run_1:
	mpirun -np 1 ./build/main.out $(x_size) $(y_size) > results_1.txt
run_2:
	mpirun -np 2 ./build/main.out $(x_size) $(y_size) > results_2.txt
run_3:
	mpirun -np 3 ./build/main.out $(x_size) $(y_size) > results_3.txt
run_4:
	mpirun -np 4 ./build/main.out $(x_size) $(y_size) > results_4.txt


