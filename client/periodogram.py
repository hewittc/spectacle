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

targetfreq = 434000000        # target frequency
#targetfreq = 98000000        # target frequency
fftsize    = 4096*4             # number of samples per transform

samplerate = 20000000          # samples per second
sampletime = 1.0 / samplerate # elapsed time between samples
samplesize = 131072           # number of samples in buffer
sampletype = np.int8          # np.uint8 for hackrf beta

samplefreq = fftfreq(fftsize, sampletime)                  # get DFT sample frequencies
samplefreq = np.array((samplefreq + targetfreq) / 1000000) # offset by target frequency
samplefreq = fftshift(samplefreq)                          # shift zero-frequency component to center

class Periodogram(Process):
    def __init__(self, pipe, events):
        super(Periodogram, self).__init__()

        self.pipe = pipe
        self.events = events

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
        self.numresults = int(samplesize / fftsize)
        self.results = np.zeros(shape=(self.numresults, fftsize), dtype=np.float64)

        # create file read chunk array
        self.chunk = np.empty(shape=(samplesize,), dtype=np.complex64)
        self.chunkpos = self.numresults
        self.chunksize = samplesize * 2

    def _read_chunk(self):
        # take a sip from the file stream
        self.events['reading'].set()
        chunk = np.frombuffer(self.pipe.recv_bytes(self.chunksize), dtype=sampletype, count=self.chunksize).astype(np.float32).view(np.complex64)
        self.events['reading'].clear()
        return chunk

    def _next_result(self):
        if self.chunkpos >= self.numresults:
            # read a new chunk if last chunk was consumed
            self.chunk[...] = self._read_chunk()
            self.chunkpos = 0

        # create a window
        start = self.chunkpos * fftsize
        end = (self.chunkpos + 1) * fftsize
        self.chunkpos += 1

        # apply windowing function to window and normalize
        iqwindowed = self.chunk[start:end] * sig.kaiser(fftsize, 14)
        iqwindowed /= (255/2)
        iqwindowed -= (1.0 + 1.0j)
        return np.log10(np.absolute(fft(iqwindowed)))

    def next(self):
        # calculate rolling average
        self.results = np.roll(self.results, shift=1, axis=0)
        self.results[0][...] = self._next_result()
        return fftshift(np.sum(self.results, axis=0)) / self.numresults

    def run(self):
        self.pt, = self.ax.plot(samplefreq, self.next())
        self.ax.set_ylim(-4.,8.)

        def next_event():
            self.pt.set_ydata(self.next())
            self.can.draw()
            return True
    
        gobject.timeout_add(60, next_event)
    
        self.win.show_all()
        gtk.main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
