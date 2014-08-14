#include "common.h"
#include "device.h"
#include "device_hackrf.h"

device_iface_t devices[] = {
	{ NULL, NULL, NULL }, 						/* DUMMY */
	{ device_hackrf_init, device_hackrf_rx, device_hackrf_tx } 	/* HACKRF */
};

