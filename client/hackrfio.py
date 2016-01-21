import common
import sys
import time
import numpy as np

from multiprocessing import Process, Pipe

try:
    from pylibhackrf import ffi, lib
except ImportError as e:
    print('error: could not load pylibhackrf')
    sys.exit(-1)

hackrf_error = {
    0: 'HACKRF_SUCCESS',
    1: 'HACKRF_TRUE',
    -2: 'HACKRF_ERROR_INVALID_PARAM',
    -5: 'HACKRF_ERROR_NOT_FOUND',
    -6: 'HACKRF_ERROR_BUSY',
    -11: 'HACKRF_ERROR_NO_MEM',
    -1000: 'HACKRF_ERROR_LIBUSB',
    -1001: 'HACKRF_ERROR_THREAD',
    -1002: 'HACKRF_ERROR_STREAMING_THREAD_ERR',
    -1003: 'HACKRF_ERROR_STREAMING_STOPPED',
    -1004: 'HACKRF_ERROR_STREAMING_EXIT_CALLED',
    -9999: 'HACKRF_ERROR_OTHER',
}

@ffi.def_extern()
def rx_callback(transfer):
    lib.hackrf_rx_buffer = transfer.buffer;
    lib.hackrf_rx_buffer_length = transfer.valid_length;
    return 0

class HackRFIO(Process):
    def __init__(self, pipe, events):
        super(HackRFIO, self).__init__()

        self.pipe = pipe
        self.events = events

        self.dev = ffi.new('hackrf_device *[1]')

        res = lib.hackrf_init()
        print('hackrf_init = {err}'.format(err=hackrf_error[res]))

        res = lib.hackrf_open(self.dev)
        print('hackrf_open = {err}'.format(err=hackrf_error[res]))

        res = lib.hackrf_set_sample_rate(self.dev[0], common.samplerate)
        print('hackrf_set_sample_rate = {err}'.format(err=hackrf_error[res]))

        res = lib.hackrf_set_vga_gain(self.dev[0], common.gainvga)
        print('hackrf_set_vga_gain = {err}'.format(err=hackrf_error[res]))

        res = lib.hackrf_set_lna_gain(self.dev[0], common.gainlna)
        print('hackrf_set_lna_gain = {err}'.format(err=hackrf_error[res]))

        res = lib.hackrf_set_freq(self.dev[0], common.targetfreq)
        print('hackrf_set_freq = {err}'.format(err=hackrf_error[res]))

    def _read_chunk(self):
        chunk = np.frombuffer(ffi.buffer(lib.hackrf_rx_buffer, lib.hackrf_rx_buffer_length), dtype=common.sampletype)
        result = chunk[::2] + 1.0j * chunk[1::2]
        return result[:1024]

    def run(self):
        res = lib.hackrf_start_rx(self.dev[0], lib.rx_callback, ffi.NULL)
        print('hackrf_start_rx = {err}'.format(err=hackrf_error[res]))

        while lib.hackrf_is_streaming(self.dev[0]):
            if self.events['reading'].is_set():
                self.pipe.send(self._read_chunk())
            else:
                self._read_chunk()

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python

