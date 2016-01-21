import common
import sys
import bz2
import gzip
import time
import numpy as np

from multiprocessing import Process, Pipe

class FileIO(Process):
    def __init__(self, path, pipe, events):
        super(FileIO, self).__init__()

        self.path = path
        self.pipe = pipe
        self.events = events

        self.fifo = False

        if self.path.endswith('bz2'):
            # automatically decompress a bz2 file
            self.fin = bz2.BZ2File(self.path, 'rb')
        elif self.path.endswith('gz'):
            # automatically decompress a gzip file
            self.fin = gzip.GzipFile(self.path, 'rb')
        else:
            # assume uncompressed otherwise
            self.fin = open(self.path, 'rb')

        if common.is_fifo(self.path):
            # determine if input is a fifo
            self.fifo = True
        else:
            # get the filesize and reset position marker to beginning
            self.fin.seek(0, 2)
            self.finsize = self.fin.tell()
            self.fin.seek(0, 0)

    def _read_chunk(self):
        if not self.fifo and (self.finsize - self.fin.tell() < common.samplesize):
            # loop back when out of data
            self.fin.seek(0, 0)

        # read a chunk of data
        chunk = np.frombuffer(self.fin.read(common.samplesize * 2), dtype=common.sampletype)
        result = chunk[::2] + 1.0j * chunk[1::2]
        return result

    def run(self):
        while True:
            start = time.time()
            if self.events['reading'].is_set():
                self.pipe.send(self._read_chunk())
            else:
                self._read_chunk()
            delay = common.sampletime - (time.time() - start)
            if delay > 0.0 and delay < 1.0:
                time.sleep(delay)

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
