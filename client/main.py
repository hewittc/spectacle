#!/bin/env python2

from __future__ import print_function

from periodogram import SpectaclePeriodogram

import numpy as np
from gi.repository import Gtk
from matplotlib.figure import Figure
from matplotlib.backends.backend_gtk3agg import FigureCanvasGTK3Agg as FigureCanvas

class SpectacleWindowMain(Gtk.Window):

    def __init__(self):
        Gtk.Window.__init__(self, title='spectacle')
        self.connect('delete-event', Gtk.main_quit)
        self.set_default_size(400, 300)

        sp = SpectaclePeriodogram()
        self.add(sp.get_canvas())

        self.show_all()
        Gtk.main()

if __name__ == '__main__':
    swm = SpectacleWindowMain()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
