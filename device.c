#include "common.h"
#include "device.h"
#include "device_iqfile.h"

device_iface devices[] = {
	{ dev_iqfile_setup, dev_iqfile_rx, dev_iqfile_tx, dev_iqfile_destroy, dev_iqfile_printf },	/* IQFILE */
};

