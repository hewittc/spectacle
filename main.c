#include "common.h"
#include "device.h"

int main(int argc, char **argv)
{
	device_iface_t *dev_iface = &devices[IQFILE];

	device_t *dev = NULL;
	dev = malloc(sizeof(*dev));

	if (dev) {
		dev_iface->config(dev, 92e6, 20e6);
		device_iqfile_open(dev, "./pager.iq", true);
		dev_iface->xfer(dev);
		device_iqfile_close(dev);
	}

	return 0;
}

