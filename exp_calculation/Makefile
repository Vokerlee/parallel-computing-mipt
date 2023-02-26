all:
	mkdir -p build
	mpicc task_exp.c -O2 -lgmp -lm -o ./build/task_exp.out

clean:
	rm -rf build

run:
	time mpirun -np 4 ./build/task_exp.out 1000000 > ./build/exp.txt
