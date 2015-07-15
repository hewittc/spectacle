#ifndef DEVICE_HACKRF_H
#define DEVICE_HACKRF_H

#include "device.h"
#include <libhackrf/hackrf.h>

#define HACKRF_DEFAULT_FREQ 89.5e6
#define HACKRF_DEFAULT_RATE 8e6

#define HACKRF_BUFFER_SIZE (262144 / 2)

int dev_hackrf_setup(device *, const uint64_t, const uint64_t);
int dev_hackrf_xfer(device *);

int rx_callback(hackrf_transfer *);
int tx_callback(hackrf_transfer *);

#endif /* DEVICE_HACKRF_H */
