#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"

typedef enum {
	IQFILE = 0,
	HACKRF = 1,
} device_type;

typedef enum {
        MODE_OFF = 0,
        MODE_RX = 1,
        MODE_TX = 2
} device_mode;

typedef struct {
	void *driver;
	device_type type;
	device_mode mode;
	uint64_t freq;		/* Hz */
	uint64_t rate;		/* Hz */
	float complex *buffer;
} device;

typedef struct {
	int (*setup)(device *, const uint64_t, const uint64_t);
	int (*xfer)(device *);
	int (*destroy)(device *);
} device_iface;

extern device_iface devices[];

int dev_destroy(device *dev);
int printf_iq(uint8_t, uint64_t);

extern int dev_iqfile_open(device *, const char *, bool);
extern int dev_iqfile_close(device *);

#endif /* DEVICE_H */

