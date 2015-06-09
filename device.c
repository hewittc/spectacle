#include "common.h"
#include "device.h"
#include "device_hackrf.h"

device_iface_t devices[] = {
	{ NULL, NULL },					/* DUMMY */
	{ device_hackrf_config, device_hackrf_xfer },	/* HACKRF */
};
