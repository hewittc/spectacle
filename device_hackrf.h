#ifndef DEVICE_HACKRF_H
#define DEVICE_HACKRF_H

#include <hackrf.h>

int device_hackrf_init(device_t*);
int device_hackrf_rx(device_t*);
int device_hackrf_tx(device_t*);

#endif /* DEVICE_HACKRF_H */
