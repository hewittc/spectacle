#ifndef DEVICE_IQFILE_H
#define DEVICE_IQFILE_H

#include "device.h"

#define IQFILE_DEFAULT_FREQ 89.5e6
#define IQFILE_DEFAULT_RATE 8e6

#define IQFILE_BUFFER_SIZE (262144 / 2)

int device_iqfile_config(device_t *, const uint64_t, const uint64_t);
int device_iqfile_xfer(device_t *);

int device_iqfile_path(device_t *, const char *);

#endif /* DEVICE_IQFILE_H */
