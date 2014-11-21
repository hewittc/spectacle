import stat
import os

def is_fifo(path):
    return stat.S_ISFIFO(os.stat(path).st_mode)

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python
