#!/bin/env python2
# hackrf_transfer -r data/data.fifo -f 434000000 -s 8000000 -l 16 -g 20

import common
import sys
import time
import logging
import multiprocessing
from fileio import FileIO
from periodogram import Periodogram
from multiprocessing import Pipe, Event

targetfreq = 434000000                              # target frequency
samplerate = 20000000                                # samples per second
samplesize = 131072                                 # number of samples in buffer
sampletype = 'int8'                                 # uint8 for hackrf beta
fftsize    = 4096*4                                   # number of samples per transform

class Spectacle:
    def __init__(self, path):
        self.path = path
        self.params = { 
            'targetfreq': targetfreq,
            'samplerate': samplerate,
            'samplesize': samplesize,
            'sampletype': sampletype,
            'fftsize'   : fftsize,
        }
        self.source, self.sink = Pipe()
        self.events = { 'reading': Event() }

        self.reader = FileIO(self.path, self.params, self.source, self.events)
        self.reader.start()

        self.periodogram = Periodogram(self.params, self.sink, self.events)
        self.periodogram.start()

        try:
            self.reader.join()
            self.periodogram.join()
        except KeyboardInterrupt:
            self.reader.terminate()
            self.periodogram.terminate()

def main():
    multiprocessing.log_to_stderr()
    logger = multiprocessing.get_logger()
    logger.setLevel(logging.INFO)

    path = ''
    if len(sys.argv) != 2:
        print('error: wrong number of arguments.')
        print('usage: {} <file>'.format(sys.argv[0]))
        sys.exit(1)
    else:
        path = sys.argv[1]

    spectacle = Spectacle(path)

if __name__ == '__main__':
    main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
