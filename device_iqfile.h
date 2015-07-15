#ifndef DEVICE_IQFILE_H
#define DEVICE_IQFILE_H

#include "device.h"

#define IQFILE_DEFAULT_FREQ 89.5e6
#define IQFILE_DEFAULT_RATE 8e6

#define IQFILE_BUFFER_SIZE (262144 / 2)

typedef struct {
        char *path;
        FILE *fp;
        bool loop;
} dev_iqfile;

int dev_iqfile_setup(device *, const uint64_t, const uint64_t);
int dev_iqfile_xfer(device *);

int dev_iqfile_path(device *, const char *);
int dev_iqfile_rx(device *);

#endif /* DEVICE_IQFILE_H */
