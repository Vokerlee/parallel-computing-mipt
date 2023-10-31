#!/bin/bash

folder_results=results
mkdir -p ${folder_results}

echo "program_num,exec_time,n_parallel" > ${folder_results}/parallel_boost.csv

make build -C program_1 DEFS="ISIZE=15000 JSIZE=15000"
make build -C program_3 DEFS="ISIZE=10000 JSIZE=10000"

threads_list="1 2 3 4 5 6 7 8"

for n_threads in ${threads_list}; do
    make build -C program_2 DEFS="ISIZE=15000 JSIZE=15000 N_THREADS=${n_threads}"

    make launch -C program_1 -s N_THREADS="${n_threads}" > ${folder_results}/program_1_${n_threads}.res
    exec_time=$(awk 'END{print $NF}' ${folder_results}/program_1_${n_threads}.res)
    echo "1,${exec_time},${n_threads}" >> ${folder_results}/parallel_boost.csv

    make launch -C program_2 -s > ${folder_results}/program_2_${n_threads}.res
    exec_time=$(awk 'END{print $NF}' ${folder_results}/program_2_${n_threads}.res)
    echo "2,${exec_time},${n_threads}" >> ${folder_results}/parallel_boost.csv

    make launch -C program_3 -s N_THREADS="${n_threads}" > ${folder_results}/program_3_${n_threads}.res
    exec_time=$(awk 'END{print $NF}' ${folder_results}/program_3_${n_threads}.res)
    echo "3,${exec_time},${n_threads}" >> ${folder_results}/parallel_boost.csv
done

rm -rf ${folder_results}/*.res

make clean -C program_1
make clean -C program_2
make clean -C program_3
