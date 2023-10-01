#!/bin/bash

folder_results=results
mkdir -p ${folder_results}
echo "exec_time,n_threads,matrix_size,optimization" > ${folder_results}/cmp.csv

for ((n_threads=1; n_threads<=8; n_threads++)); do
    for ((matrix_size=32; matrix_size<=512; matrix_size=matrix_size*2)); do
        make build DEFS="MATRIX_SIZE=${matrix_size} N_THREADS=${n_threads}"
        make run_default > ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res
        exec_time=$(awk 'END{print $NF}' ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res)
        echo "${exec_time},${n_threads},${matrix_size},none" >> ${folder_results}/cmp.csv
    done
done
