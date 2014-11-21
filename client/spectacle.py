#!/bin/env python2
# hackrf_transfer -r data/data.fifo -f 434000000 -s 8000000 -l 16 -g 20

import bz2
import gzip
import struct
import sys
import time
import common

import numpy as np
from numpy import random as rand
from scipy.fftpack import fft, fftfreq, fftshift
from scipy import signal as sig

from matplotlib import pyplot, animation

targetfreq = 434000000        # target frequency
fftsize    = 4096             # number of samples per transform

samplerate = 2000000          # samples per second
sampletime = 1.0 / samplerate # elapsed time between samples
samplesize = 131072           # number of samples in buffer
sampletype = np.int8          # np.uint8 for hackrf beta

samplefreq = fftfreq(fftsize, sampletime)                  # get DFT sample frequencies
samplefreq = np.array((samplefreq + targetfreq) / 1000000) # offset by target frequency
samplefreq = fftshift(samplefreq)                          # shift zero-frequency component to center


class FFTData:
    def __init__(self, path):
        self.fifo = False

        if common.is_fifo(path):
            # determine if input is a fifo
            self.fifo = True

        if path.endswith('bz2'):
            # automatically decompress a bz2 file
            self.fin = bz2.BZ2File(path, 'rb')
        elif path.endswith('gz'):
            # automatically decompress a gzip file
            self.fin = gzip.GzipFile(path, 'rb')
        else:
            # assume uncompressed otherwise
            self.fin = open(path, 'rb')

        if not self.fifo:
            # get the filesize and reset position marker to beginning
            self.fin.seek(0, 2)
            self.finsize = self.fin.tell()
            self.fin.seek(0, 0)
       
        # create file chunk arrays
        self.chunksize = samplesize * 2
        self.chunk = np.empty(shape=(samplesize,), dtype=np.complex64)
        self.chunk[...] = self._next_chunk()
        self.chunkpos = 0

        # create result buffer
        self.numresults = int(samplesize / fftsize)
        self.results = np.zeros(shape=(self.numresults, fftsize), dtype=np.float64)
        
    def _next_chunk(self):
        if not self.fifo and (self.finsize - self.fin.tell() < self.chunksize):
            # loop back when out of data
            self.fin.seek(0, 0)

        # read a chunk of data and deinterleave I/Q
        return np.frombuffer(self.fin.read(self.chunksize), dtype=sampletype, count=self.chunksize).astype(np.float32).view(np.complex64)

    def _next_result(self):
        if self.chunkpos >= self.numresults:
            # read a new chunk if last chunk was consumed
            self.chunk[...] = self._next_chunk()
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
        

def main():
    import gtk, gobject
    from matplotlib.figure import Figure
    from matplotlib.backends.backend_gtkagg import \
        FigureCanvasGTKAgg as FigureCanvas
    from matplotlib.backends.backend_gtkagg import \
        NavigationToolbar2GTKAgg as NavigationToolbar
    
    win = gtk.Window(gtk.WINDOW_TOPLEVEL)
    win.connect('destroy', lambda x: gtk.main_quit())
    win.connect('delete_event', lambda x,y: False)
    
    win.set_default_size(600,350)
    win.set_title('spectacle')
    
    fig = Figure(figsize=(6,3),dpi=100)
    ax = fig.add_subplot(1,1,1)
    can = FigureCanvas(fig)
    tbar = NavigationToolbar(can,None)
    vbox = gtk.VBox()
    vbox.pack_start(tbar, False, False, 0)
    vbox.pack_start(can, True, True, 0)
    win.add(vbox)
    
    path = ''
    if len(sys.argv) != 2:
        print('error: wrong number of arguments.')
        print('usage: {} <file>'.format(sys.argv[0]))
        sys.exit(1)
    else:
        path = sys.argv[1]

    fftdata = FFTData(path)
    pt, = ax.plot(samplefreq, fftdata.next())
    ax.set_ylim(-4.,8.)

    def next_event():
        can,tbar = vbox.get_children()
        pt.set_ydata(fftdata.next())
        #ax.set_ylim(0.9*np.min(pt.get_ydata()),1.1*np.max(pt.get_ydata()))
        can.draw()
        return True
    
    gobject.timeout_add(60, next_event)
    
    win.show_all()
    gtk.main()

if __name__ == '__main__':
    main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
