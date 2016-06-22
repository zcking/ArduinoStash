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


def plot_data(file_path):
    with open(file_path, 'r') as f:
        entries = f.read().splitlines() # read all the lines into a list
        start = float(entries[0].split(':')[0]) # get first second
        end = float(entries[-1].split(':')[0]) # get last second

        values = []
        for entry in entries:
            values.append(int(entry.split(':')[1]))

        plt.plot(values)
        plt.axis([start, end, min(values)-10, max(values)+10])
        plt.ylabel('Quantity of Received CAN Messages')
        plt.xlabel('Time (s)')
        plt.title('Real-time Operation of \nSimulative Controlled Area Network \n(Control Group)')
        plt.show()

def plot_two_data(file_path1, file_path2):
    with open(file_path1, 'r') as f:
        entries1 = f.read().splitlines() # read all the lines into a list

    with open(file_path2, 'r') as f:
        entries2 = f.read().splitlines()

    start = min(float(entries1[0].split(':')[0]), float(entries2[0].split(':')[0])) 
    end = max(float(entries1[-1].split(':')[0]), float(entries2[-1].split(':')[0]))

    values1 = []
    values2 = []
    for entry in entries1:
        values1.append(int(entry.split(':')[1]))
    for entry in entries2:
        values2.append(int(entry.split(':')[1]))

    plt.plot(values1)
    plt.plot(values2)
    plt.axis([start, end, min(values1+values2)-10, max(values1+values2)+10])
    plt.ylabel('Quantity of Received CAN Messages')
    plt.xlabel('Time (s)')
    plt.title('Real-time Operation of \nSimulative Controlled Area Network \n(Control Group)')
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: python plot_log.py <logfile-path>')
        sys.exit(1)
    elif len(sys.argv) == 3:
        plot_two_data(sys.argv[1], sys.argv[2])
    else:
        plot_data(sys.argv[1])