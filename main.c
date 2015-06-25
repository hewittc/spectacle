#include "common.h"
#include "device.h"
#include "server.h"

int main(int argc, char **argv)
{
	device_iface_t *dev_iface = &devices[IQFILE];

	device_t *dev = NULL;
	dev = (device_t *) malloc(sizeof(device_t *));

	start_server("tcp://*:5555");

	if (dev) {
		dev_iface->config(dev, 92e6, 20e6);
		dev_iface->xfer(dev);
	}

	return 0;
}

