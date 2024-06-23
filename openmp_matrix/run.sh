#!/bin/bash

folder_results=results
mkdir -p ${folder_results}

echo "exec_time,n_threads,matrix_size,optimization" > ${folder_results}/cmp_opt_offload.csv

matrix_size_list="256 512 1024 2048 3072 4096"
threads_list="1 2 4 8 16 24"

for n_threads in ${threads_list}; do
    for matrix_size in ${matrix_size_list}; do
        make build DEFS="MATRIX_SIZE=${matrix_size} N_THREADS=${n_threads}"

        # make run_default > ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/default_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},none" >> ${folder_results}/cmp.csv

        # make run_opt1 > ${folder_results}/opt1_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/opt1_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},opt1_loop" >> ${folder_results}/cmp.csv

        # make run_opt2 > ${folder_results}/opt2_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/opt2_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},opt2" >> ${folder_results}/cmp_opt2.csv

        # make run_opt3 > ${folder_results}/opt3_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/opt3_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},opt_3" >> ${folder_results}/cmp_opt3.csv

        # make run_opt4 > ${folder_results}/opt4_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/opt4_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},opt_4" >> ${folder_results}/cmp_opt4.csv

        # make run_strassen > ${folder_results}/opt_strassen_matrix_${matrix_size}_${n_threads}.res
        # exec_time=$(awk 'END{print $NF}' ${folder_results}/opt_strassen_matrix_${matrix_size}_${n_threads}.res)
        # echo "${exec_time},${n_threads},${matrix_size},strassen" >> ${folder_results}/cmp_opt_strassen.csv

        make run_offload > ${folder_results}/opt_offload_matrix_${matrix_size}_${n_threads}.res
        exec_time=$(awk 'END{print $NF}' ${folder_results}/opt_offload_matrix_${matrix_size}_${n_threads}.res)
        echo "${exec_time},${n_threads},${matrix_size},offload" >> ${folder_results}/cmp_opt_offload.csv
    done
done
