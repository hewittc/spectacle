#include "common.h"
#include "device.h"

int main(int argc, char **argv)
{
	device_iface *dev_iface = &devices[IQFILE];

	device *dev = calloc(1, sizeof(*dev));

	if (dev) {
		dev_iface->setup(dev, 92e6, 20e6);
		dev_iqfile_open(dev, "./pager.iq", false);
		dev_iface->rx(dev, 100);
		dev_iqfile_close(dev);
		dev_iface->destroy(dev);
	}

	return 0;
}

