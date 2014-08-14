#ifndef DEVICE_HACKRF_H
#define DEVICE_HACKRF_H

#include <hackrf.h>

typedef enum {
	TRANSCEIVER_MODE_OFF = 0,
	TRANSCEIVER_MODE_RX = 1,
	TRANSCEIVER_MODE_TX = 2,
	TRANSCEIVER_MODE_SS = 3
} transceiver_mode_t;

int device_hackrf_init(device_t*);
int device_hackrf_rx(device_t*);
int device_hackrf_tx(device_t*);

#endif /* DEVICE_HACKRF_H */
