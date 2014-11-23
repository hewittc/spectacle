#!/bin/env python2
# hackrf_transfer -r data/data.fifo -f 434000000 -s 8000000 -l 16 -g 20

import sys
import time
import common
import gtk
import gobject
import numpy as np
from multiprocessing import Process, Pipe, Event
from numpy import random as rand
from scipy.fftpack import fft, fftfreq, fftshift
from scipy import signal as sig
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar
from matplotlib import pyplot, animation

class Periodogram(Process):
    def __init__(self, params, pipe, events):
        super(Periodogram, self).__init__()

        self.params = params
        self.pipe = pipe
        self.events = events

        self.sampletime = 1.0 / self.params['samplerate']                  # elapsed time between samples
        self.samplefreq = fftfreq(self.params['fftsize'], self.sampletime) # get DFT sample frequencies
        self.samplefreq = np.array((self.samplefreq + \
            self.params['targetfreq']) / 1000000)                          # offset by target frequency
        self.samplefreq = fftshift(self.samplefreq)                        # shift zero-frequency component to center

        self.win = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.win.connect('destroy', lambda x: gtk.main_quit())
        self.win.connect('delete_event', lambda x,y: False)
    
        self.win.set_default_size(600,350)
        self.win.set_title('spectacle')
    
        self.fig = Figure(figsize=(6,3), dpi=100)
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.can = FigureCanvas(self.fig)
        self.tbar = NavigationToolbar(self.can, None)

        self.vbox = gtk.VBox()
        self.vbox.pack_start(self.tbar, False, False, 0)
        self.vbox.pack_start(self.can, True, True, 0)

        self.win.add(self.vbox)

        # create result buffer
        self.numresults = int(self.params['samplesize'] / self.params['fftsize'])
        self.results = np.zeros(shape=(self.numresults, self.params['fftsize']), dtype=np.float64)

        # create file read chunk array
        self.chunk = np.empty(shape=(self.params['samplesize'],), dtype=np.complex64)
        self.chunkpos = self.numresults
        self.chunksize = self.params['samplesize'] * 2

    def _read_chunk(self):
        # take a sip from the file stream
        self.events['reading'].set()
        chunk = np.frombuffer(self.pipe.recv_bytes(self.chunksize), 
            dtype=self.params['sampletype'], count=self.chunksize).astype(np.float32).view(np.complex64)
        self.events['reading'].clear()
        return chunk

    def _next_result(self):
        if self.chunkpos >= self.numresults:
            # read a new chunk if last chunk was consumed
            self.chunk[...] = self._read_chunk()
            self.chunkpos = 0

        # create a window
        start = self.chunkpos * self.params['fftsize']
        end = (self.chunkpos + 1) * self.params['fftsize']
        self.chunkpos += 1

        # apply windowing function to window and normalize
        iqwindowed = self.chunk[start:end] * sig.kaiser(self.params['fftsize'], 14)
        iqwindowed /= (255/2)
        iqwindowed -= (1.0 + 1.0j)
        return np.log10(np.absolute(fft(iqwindowed)))

    def next(self):
        # calculate rolling average
        self.results = np.roll(self.results, shift=1, axis=0)
        self.results[0][...] = self._next_result()
        return fftshift(np.sum(self.results, axis=0)) / self.numresults

    def next_event(self):
        self.pt.set_ydata(self.next())
        self.can.draw()
        return True
 
    def run(self):
        self.pt, = self.ax.plot(self.samplefreq, self.next())
        self.ax.set_xlim(self.samplefreq[0], self.samplefreq[len(self.samplefreq) - 1])
        self.ax.set_ylim(-4.,8.)

        gobject.timeout_add(60, self.next_event)
        self.win.show_all()

        gtk.main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
