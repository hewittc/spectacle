#include "common.h"
#include "device.h"
#include "device_hackrf.h"
#include "device_iqfile.h"

device_iface_t devices[] = {
	{ NULL, NULL },					/* DUMMY */
	{ device_iqfile_config, device_iqfile_xfer },	/* IQFILE */
	{ device_hackrf_config, device_hackrf_xfer },	/* HACKRF */
};
