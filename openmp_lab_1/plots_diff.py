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

class TimingsResult:
    def __init__(self, exec_time, n_threads):
        self.exec_time = exec_time
        self.n_threads = n_threads

####################################################################################

def get_index_by_el(list, element):
    for index, value in enumerate(list):
        if value == element:
            return index

    return -1

def parse_csv(filename):
    prog_nums = set()

    with open(filename, 'r') as file:
        csv_reader = csv.DictReader(file)
        for line in csv_reader:
            prog_nums.add(int(line['program_num']))

    prog_nums = sorted(list(prog_nums))

    calc_timings = []
    for prog_num in range(len(prog_nums)):
        calc_timings.append([])

    with open(filename, 'r') as file:
        csv_reader = csv.DictReader(file)
        for line in csv_reader:
            prog_num = get_index_by_el(prog_nums, int(line['program_num']))
            calc_timings[prog_num].append(TimingsResult(float(line['exec_time']), float(line['n_parallel'])))

    return calc_timings, prog_nums

def plot_boosts(calc_timings, prog_nums):
    figure = plt.figure(figsize=(FIGURE_XSIZE, FIGURE_YSIZE), facecolor=BACKGROUND_RGB)
    gs = GridSpec(ncols=1, nrows=1, figure=figure)
    axes = figure.add_subplot(gs[0, 0])
    set_axis_properties(axes)

    axes.set_xlabel('Amount of threads/processes')
    axes.set_ylabel('Boost')
    axes.set_title('Boost from amount of threads')

    for prog_num in range(len(prog_nums)):
        n_threads = [calc_timings[prog_num][i].n_threads
                        for i in range(len(calc_timings[prog_num]))]

        exec_time = [calc_timings[prog_num][0].exec_time / \
                     calc_timings[prog_num][i].exec_time
                        for i in range(len(calc_timings[prog_num]))]

        axes.plot(n_threads, exec_time, "*-", label=("program #" + str(prog_nums[prog_num])))

    axes.legend()
    figure.tight_layout()

    plt.savefig('boosts.png')

####################################################################################

# SCRIPT START

if len(sys.argv) < 2:
    print("Invalid amount of arguments [at least 1 required]", file=sys.stderr)
    exit(1)

data_filename = sys.argv[1]
calc_timings, prog_nums = parse_csv(data_filename)
plot_boosts(calc_timings, prog_nums)
