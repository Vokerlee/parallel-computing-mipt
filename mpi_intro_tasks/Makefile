all:
	mkdir -p build

	mpicc task1.c -O2 -o ./build/task1.out
	mpicc task2.c -O2 -o ./build/task2.out
	mpicc task3.c -O2 -o ./build/task3.out

clean:
	rm -rf build

run1:
	mpirun -np 4 ./build/task1.out
