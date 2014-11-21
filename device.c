#include "common.h"
#include "device.h"
#include "device_hackrf.h"
#include "device_uhd.h"

device_iface_t devices[] = {
	{ NULL, NULL },					/* DUMMY */
	{ device_hackrf_config, device_hackrf_xfer },	/* HACKRF */
	{ device_uhd_config, device_uhd_xfer }		/* UHD */
};
