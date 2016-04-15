import sys
import time
import common

try:
    import gtk
    import gobject
except ImportError as e:
    print('error: install python-gtk and python-gobject')
    sys.exit(-1)

try:
    import numpy as np
    from numpy import random as rand

    from scipy.fftpack import fft, fftfreq, fftshift
    from scipy import signal as sig
    from matplotlib.figure import Figure
    from matplotlib.backends.backend_gtkagg import FigureCanvasGTKAgg as FigureCanvas
    from matplotlib.backends.backend_gtkagg import NavigationToolbar2GTKAgg as NavigationToolbar
    from matplotlib import pyplot, animation
except ImportError as e:
    print('error: install numpy')
    sys.exit(-1)

from multiprocessing import Process, Pipe, Event

class Periodogram(Process):
    def __init__(self, pipe, events):
        super(Periodogram, self).__init__()

        self.pipe = pipe
        self.events = events

        self.sampletime = 1.0 / common.samplerate                                   # elapsed time between samples
        self.samplefreq = fftfreq(common.samplesize, self.sampletime)               # get dft sample frequencies
        self.samplefreq = np.array((self.samplefreq + common.targetfreq) / 1000000) # offset by target frequency
        self.samplefreq = fftshift(self.samplefreq)                                 # shift zero-frequency component to center

        self.win = gtk.Window(gtk.WINDOW_TOPLEVEL)

        self.win.connect('destroy', lambda x: gtk.main_quit())
        self.win.connect('delete_event', lambda x,y: False)
    
        self.win.set_default_size(800,350)
        self.win.set_title('spectacle')
    
        self.fig = Figure(figsize=(6,3), dpi=100)
        self.ax = self.fig.add_subplot(1, 1, 1)
        self.can = FigureCanvas(self.fig)
        self.tbar = NavigationToolbar(self.can, None)

        self.vbox = gtk.VBox()
        self.vbox.pack_start(self.tbar, False, False, 0)
        self.vbox.pack_start(self.can, True, True, 0)

        self.win.add(self.vbox)

        # create file read chunk array
        self.chunk = np.empty(shape=(common.samplesize,), dtype=np.complex64)

    def _read_chunk(self):
        # take a sip from the file stream
        self.events['reading'].set()
        chunk = self.pipe.recv() 
        self.events['reading'].clear()
        return chunk

    def _next_result(self):
        self.chunk[...] = self._read_chunk()

        # apply windowing function to window and normalize
        self.chunk[...] *= sig.hanning(common.samplesize)
        return fftshift(10.0 * np.log10(np.absolute(fft(self.chunk))))

    def next_event(self):
        self.pt.set_ydata(self._next_result())
        self.can.draw()
        return True
 
    def run(self):
        self.pt, = self.ax.plot(self.samplefreq, self._next_result())
        self.ax.set_xlim(self.samplefreq[0], self.samplefreq[len(self.samplefreq) - 1])
        self.ax.set_ylim(-10.,40.)

        gobject.timeout_add(120, self.next_event)
        self.win.show_all()

        gtk.main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
