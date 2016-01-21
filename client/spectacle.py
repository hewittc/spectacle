#!/usr/bin/env python2
# hackrf_transfer -r data/data.fifo -f 434000000 -s 8000000 -l 16 -g 20

import common
import sys
import time
import logging
import multiprocessing

from fileio import FileIO
from hackrfio import HackRFIO
from periodogram import Periodogram
from multiprocessing import Pipe, Event

class Spectacle:
    def __init__(self, path):
        self.path = path
        self.source, self.sink = Pipe()
        self.events = { 'reading': Event() }

        if self.path is '':
            self.reader = HackRFIO(self.source, self.events)
        else:
            self.reader = FileIO(self.path, self.source, self.events)
        self.reader.start()

        self.periodogram = Periodogram(self.sink, self.events)
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
    if len(sys.argv) not in (1, 2):
        print('error: wrong number of arguments.')
        print('usage: {}          (to use hackrf)'.format(sys.argv[0]))
        print('usage: {} <file>   (to use raw file)'.format(sys.argv[0]))
        sys.exit(1)
    else:
        if len(sys.argv) is 2:
            path = sys.argv[1]

    spectacle = Spectacle(path)

if __name__ == '__main__':
    main()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
