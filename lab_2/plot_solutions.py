import math
import sys
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
import statistics
import csv
import shutil
from matplotlib.gridspec import GridSpec
from matplotlib.ticker   import MaxNLocator, AutoLocator, LinearLocator

####################################################################################

# matplotlib.rcParams['axes.formatter.limits'] = (-5, 4)

MAX_LOCATOR_NUMBER = 10
FIGURE_XSIZE = 10
FIGURE_YSIZE = 6

BACKGROUND_RGB = '#F5F5F5'
MAJOR_GRID_RGB = '#919191'

LEGEND_FRAME_ALPHA = 0.95

####################################################################################

def set_axis_properties(axes):
    axes.xaxis.set_major_locator(LinearLocator(MAX_LOCATOR_NUMBER))
    axes.yaxis.set_major_locator(LinearLocator(MAX_LOCATOR_NUMBER))
    axes.minorticks_on()
    axes.grid(which='major', linewidth=2, color=MAJOR_GRID_RGB)
    axes.grid(which='minor', linestyle=':')

####################################################################################

def get_index_by_el(list, element):
    for index, value in enumerate(list):
        if value == element:
            return index

    return -1

def parse_csv(filename):
    x = []
    Y = [[] for i in range(11)]

    with open(filename, 'r') as file:
        csv_reader = csv.reader(file)
        next(csv_reader, None)

        for line in csv_reader:
            x.append(float(line[0]))
            for i in range(len(Y)):
                Y[i].append(float(line[i + 1]))

    return x, Y

def plot_solutions(x, Y):
    figure = plt.figure(figsize=(FIGURE_XSIZE, FIGURE_YSIZE), facecolor=BACKGROUND_RGB)
    gs = GridSpec(ncols=1, nrows=1, figure=figure)
    axes = figure.add_subplot(gs[0, 0])
    set_axis_properties(axes)

    axes.set_xlabel('x')
    axes.set_ylabel('y')
    axes.set_title('Solutions for different boundaries\' conditions')

    # plt.gca().invert_yaxis()
    plt.autoscale(enable=False, axis='both')

    for i in range(len(Y)):
        axes.scatter(x, Y[i], label=("y" + str(i)))

    axes.legend()
    figure.tight_layout()

    plt.savefig('solutions.png')
    # plt.show()

####################################################################################

# SCRIPT START

if len(sys.argv) < 2:
    print("Invalid amount of arguments [at least 1 required]", file=sys.stderr)
    exit(1)

data_filename = sys.argv[1]
x, Y = parse_csv(data_filename)
plot_solutions(x, Y)
