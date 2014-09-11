#!/bin/env python2

from __future__ import print_function

import numpy as np
from gi.repository import Gtk
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg as FigureCanvas

class SpectaclePeriodogram(Gtk.Widget):

    def __init__(self):
        Gtk.Widget.__init__(self)
        figure = Figure()
        axes = figure.add_subplot(1, 1, 1)

        nums = []

        with open('foo.txt', 'r') as f:
            for line in f.readlines():
                num = line.strip().split('+j')
                nums.append(complex(float(num[0]), float(num[1])))

            arr = np.array(nums, dtype=complex)
            axes.grid(True)
            axes.set_xlabel('frequency')
            axes.set_ylabel('magnitude')
            axes.set_xlim([0, arr.size / 2])
            axes.set_ylim([0, 100])

            func = np.sqrt(arr.real**2 + arr.imag**2)
            axes.fill_between(np.arange(arr.size), func, linewidth=0, color='blue')

        self.canvas = FigureCanvas(figure)

    def get_canvas(self):
        return self.canvas

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
