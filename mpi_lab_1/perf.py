import matplotlib
import math
import numpy as np
import matplotlib.pyplot as plt
import statistics
import os
import shutil
from sklearn.linear_model import LinearRegression
from matplotlib.gridspec  import GridSpec
from matplotlib.ticker    import MaxNLocator, ScalarFormatter, FixedFormatter

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

figure = plt.figure(figsize=(FIGURE_XSIZE, FIGURE_YSIZE), facecolor=BACKGROUND_RGB)
gs = GridSpec(ncols=1, nrows=1, figure=figure)
axes = figure.add_subplot(gs[0, 0])
set_axis_properties(axes)

axes.set_xlabel('amount of cores')
axes.set_ylabel('perfromance, $s^{-1}$')
axes.set_title('performance from amount of cores')

times = [[15.6455, 9.0251, 6.5483, 5.2129], 
         [10.6281, 6.0823, 4.5127, 3.8537],
         [0.93217, 0.514878, 0.363718, 0.281629]]
labels = np.array(["9000 x 9000", "2000 x 30000", "200 x 30000"])

cores = np.array([1, 2, 3, 4])

for i in range(len(times)):
    axes.plot(cores, max(np.array(times[i])) / np.array(times[i]), label=labels[i], marker='o', markersize=6, linestyle='-')

axes.legend(framealpha=LEGEND_FRAME_ALPHA)
plt.show()
