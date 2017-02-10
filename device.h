#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"

typedef enum {
	IQFILE = 0,
} device_type;

typedef struct {
	void *context;
	device_type type;
	uint64_t freq;
	uint64_t rate;
	size_t buffer_size;
} device;

typedef struct {
	int (*setup)(device *, const uint64_t, const uint64_t);
	int (*rx)(device *, complex float *, size_t);
	int (*destroy)(device *);
} device_iface;

int printf_cbuffer(complex float *, size_t);

extern int dev_iqfile_open(device *, const char *, bool);
extern int dev_iqfile_close(device *);

extern device_iface devices[];

void *device_rx(void *);

#endif /* DEVICE_H */

