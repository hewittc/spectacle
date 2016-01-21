import stat
import os

targetfreq = int(90e6) # target frequency
samplerate = int(8e6)  # samples per second
samplesize = int(1024) # size of fft
sampletime = 1.0 / (samplerate / samplesize)

sampletype = 'int8'    # uint8 for hackrf beta

gainlna    = int(20)   # lna gain
gainvga    = int(8)    # vga gain

def is_fifo(path):
    return stat.S_ISFIFO(os.stat(path).st_mode)

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
