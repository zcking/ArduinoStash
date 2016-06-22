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
        

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('Usage: python plot_log.py <logfile-path>')
        sys.exit(1)
    else:
        for i in range(1, len(sys.argv)):
            plot_data(sys.argv[i])
    plt.show()