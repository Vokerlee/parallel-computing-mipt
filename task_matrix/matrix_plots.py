import math
import sys
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import statistics
import csv
import shutil
from matplotlib.gridspec import GridSpec
from matplotlib.ticker   import MaxNLocator

####################################################################################

matplotlib.rcParams['axes.formatter.limits'] = (-5, 4)

MAX_LOCATOR_NUMBER = 10
FIGURE_XSIZE = 10
FIGURE_YSIZE = 8

BACKGROUND_RGB = '#F5F5F5'
MAJOR_GRID_RGB = '#919191'

LEGEND_FRAME_ALPHA = 0.95

####################################################################################

def set_axis_properties(axes):
    axes.xaxis.set_major_locator(MaxNLocator(MAX_LOCATOR_NUMBER))
    axes.minorticks_on()
    axes.grid(which='major', linewidth=2, color=MAJOR_GRID_RGB)
    axes.grid(which='minor', linestyle=':')

####################################################################################

class MatrixResult:
    def __init__(self, exec_time, n_threads):
        self.exec_time = exec_time
        self.n_threads = n_threads

####################################################################################

def get_index_by_el(list, element):
    for index, value in enumerate(list):
        if value == element:
            return index

    return -1

def parse_matrix_csv(filename):
    optimizations = set()
    martix_sizes  = set()

    with open(filename, 'r') as file:
        csv_reader = csv.DictReader(file)
        for line in csv_reader:
            optimizations.add(line['optimization'])
            martix_sizes.add(int(line['matrix_size']))

    optimizations = sorted(list(optimizations))
    martix_sizes  = sorted(list(martix_sizes))

    matrix_res = []
    for opt_index in range(len(optimizations)):
        matrix_opt_res = []

        for matrix_size_index in range(len(martix_sizes)):
            matrix_opt_res.append([])

        matrix_res.append(matrix_opt_res)

    with open(filename, 'r') as file:
        csv_reader = csv.DictReader(file)
        for line in csv_reader:
            opt_index = get_index_by_el(optimizations, line['optimization'])
            matrix_size_index = get_index_by_el(martix_sizes, float(line['matrix_size']))

            matrix_res[opt_index][matrix_size_index].append(MatrixResult(float(line['exec_time']), float(line['n_threads'])))

    return matrix_res, martix_sizes, optimizations

def plot_matrix_res(matrix_res, martix_sizes, optimizations):
    figure = plt.figure(figsize=(FIGURE_XSIZE, FIGURE_YSIZE), facecolor=BACKGROUND_RGB)
    gs = GridSpec(ncols=1, nrows=1, figure=figure)
    axes = figure.add_subplot(gs[0, 0])
    set_axis_properties(axes)

    axes.set_xlabel('Amount of threads (OpenMP)')
    axes.set_ylabel('Execution time')
    axes.set_title('Execution time from amount of threads for matrix multiplication')

    none_opt_index = get_index_by_el(optimizations, 'none')

    for matrix_size_index in range(len(martix_sizes)):
        for opt_index in range(len(optimizations)):
            n_threads = [matrix_res[opt_index][matrix_size_index][i].n_threads
                         for i in range(len(matrix_res[opt_index][matrix_size_index]))]

            exec_time = [matrix_res[none_opt_index][matrix_size_index][0].exec_time / \
                         matrix_res[opt_index][matrix_size_index][i].exec_time
                         for i in range(len(matrix_res[opt_index][matrix_size_index]))]

            axes.plot(n_threads, exec_time, "*-", label=("matrix size: " + str(martix_sizes[matrix_size_index]) +
                                                       ", optimization: " + optimizations[opt_index]))


    axes.legend()
    figure.tight_layout()

    plt.savefig('cmp.png')

####################################################################################

# SCRIPT START

if len(sys.argv) < 2:
    print("Invalid amount of arguments [at least 1 required]", file=sys.stderr)
    exit(1)

matrix_data_filename = sys.argv[1]
matrix_res, martix_sizes, optimizations = parse_matrix_csv(matrix_data_filename)
plot_matrix_res(matrix_res, martix_sizes, optimizations)






