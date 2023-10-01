#!/bin/bash

folder_results=results
mkdir -p ${folder_results}
echo "exec_time,n_threads,matrix_size,optimization" > ${folder_results}/cmp.csv

matrix_size_list="128 256 384 512 640 768 996 1024"

for ((n_threads=1; n_threads<=8; n_threads++)); do
    for matrix_size in ${matrix_size_list}; do
        make build DEFS="MATRIX_SIZE=${matrix_size} N_THREADS=${n_threads}"
        make run_default > ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res
        exec_time=$(awk 'END{print $NF}' ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res)
        echo "${exec_time},${n_threads},${matrix_size},none" >> ${folder_results}/cmp.csv
    done
done
