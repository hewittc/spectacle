#ifndef DEVICE_H
#define DEVICE_H

#include "common.h"

typedef enum {
	DUMMY = 0,
	IQFILE = 1,
	HACKRF = 2,
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
} device_t;

typedef struct {
	int (*config)(device_t *, const uint64_t, const uint64_t);
	int (*xfer)(device_t *);
} device_iface_t;

typedef struct {
	char *path;
	FILE *fp;
} device_file_t;

extern device_iface_t devices[];

extern int device_iqfile_open(device_t *, const char *);
extern int device_iqfile_close(device_t *);

#endif /* DEVICE_H */

