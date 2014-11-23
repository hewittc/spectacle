import common
import sys
import bz2
import gzip
import time
import numpy as np

from multiprocessing import Process, Pipe

samplerate = 20000000         # samples per second
samplesize = 131072           # number of samples in buffer
sampletype = np.int8          # np.uint8 for hackrf beta


class FileIO(Process):
    def __init__(self, path, pipe, reading):
        super(FileIO, self).__init__()

        self.path = path
        self.pipe = pipe
        self.reading = reading

        self.fifo = False
        self.chunksize = samplesize * 2
        self.chunktime = 1.0 / (samplerate / samplesize)

    def _read_chunk(self):
        if not self.fifo and (self.finsize - self.fin.tell() < self.chunksize):
            # loop back when out of data
            self.fin.seek(0, 0)

        # read a chunk of data
        return np.frombuffer(self.fin.read(self.chunksize), dtype=sampletype, count=self.chunksize)

    def run(self):
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

        while True:
            delay = time.time()
            if self.reading.is_set():
                print('hello')
                self.pipe.send_bytes(self._read_chunk())
            else:
                print('goodbye')
                self._read_chunk()
            delay = self.chunktime - (time.time() - delay)
            if delay > 0.0 and delay < 1.0:
                time.sleep(delay)

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
