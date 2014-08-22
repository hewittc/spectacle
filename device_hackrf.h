#ifndef DEVICE_HACKRF_H
#define DEVICE_HACKRF_H

#include "device.h"
#include <hackrf.h>

#define DEFAULT_FREQ 89.5e6
#define DEFAULT_RATE 8e6

int device_hackrf_config(device_t*, const uint64_t, const uint64_t);
int device_hackrf_xfer(device_t*);

int rx_callback(hackrf_transfer*);
int tx_callback(hackrf_transfer*);

#endif /* DEVICE_HACKRF_H */
