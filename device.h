#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"

typedef enum {
	IQFILE = 0,
} device_type;

typedef struct {
	void *driver;
	device_type type;
	uint64_t freq;		/* Hz */
	uint64_t rate;		/* Hz */
	float complex *buffer;
} device;

typedef struct {
	int (*setup)(device *, const uint64_t, const uint64_t);
	int (*rx)(device *, size_t samples);
	int (*tx)(device *);
	int (*destroy)(device *);
	int (*printf)(device *, size_t);
} device_iface;

extern device_iface devices[];

extern int dev_iqfile_open(device *, const char *, bool);
extern int dev_iqfile_close(device *);

#endif /* DEVICE_H */

