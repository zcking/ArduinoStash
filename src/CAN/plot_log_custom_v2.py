"""
File: plot_log.py
Author: Zachary King
6/22/2016
---
Plots data from log files produced by 
the CAN Arduino programs.
"""

import matplotlib.pyplot as plt
import numpy as np
import sys

minimx = 999
maximx = 0
minimy = 999
maximy = 0

average = 0
total = 0
counter = 0

legend_handles = []

def plot_data(file_path, label=None):
    global minimx, minimy, maximx, maximy, average, total, counter, legend_handles
    
    prev = 0

    with open(file_path, 'r') as f:
        entries = f.read().splitlines() # read all the lines into a list
        start = float(entries[0].split(':')[0]) # get first second
        end = float(entries[-1].split(':')[0]) # get last second

        counter += (end - start)
        values = []
        for entry in entries:
            values.append(prev + int(entry.split(':')[1]))
            total += (int(entry.split(':')[1]))
            prev += int(entry.split(':')[1])
        average = total / counter

        minimx = min(minimx, start)
        maximx = max(maximx, end)
        minimy = min(minimy, min(values))
        maximy = max(maximy, max(values))

        l, = plt.plot(values, label=label)
        legend_handles.append(l)
        plt.axis([minimx, maximx, minimy, maximy])
        

if __name__ == '__main__':
    if len(sys.argv) < 5:
        print('Usage: python plot_log.py <title> <x-axis-label> <y-axis-label> <log-filepath-1> <label-1> ...*')
        sys.exit(1)
    else:
        for i in range(4, len(sys.argv), 2):
            plot_data(sys.argv[i], label=sys.argv[i+1])
        plt.title(sys.argv[1])
        plt.xlabel(sys.argv[2])
        plt.ylabel(sys.argv[3])
        plt.legend(handles=legend_handles, bbox_to_anchor=(0.6, 0.05, 0, 0), loc=3,
           ncol=1)


    print(average)
    plt.show()