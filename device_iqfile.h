#ifndef DEVICE_IQFILE_H
#define DEVICE_IQFILE_H

#include "device.h"

#define IQFILE_DEFAULT_FREQ 89.5e6
#define IQFILE_DEFAULT_RATE 8e6

#define IQFILE_BUFFER_SIZE  262144

typedef struct {
        char *path;
        FILE *fp;
	uint64_t pos;
        bool loop;
} dev_iqfile;

int dev_iqfile_setup(device *, const uint64_t, const uint64_t);
int dev_iqfile_rx(device *, complex float *, size_t);
int dev_iqfile_destroy(device *);

int dev_iqfile_path(device *, const char *);

int dev_iqfile_printf(device *, size_t);
int dev_iqfile_cprintf(device *, size_t);

int dev_iqfile_open(device *, const char *, const bool);
int dev_iqfile_close(device *);

#endif /* DEVICE_IQFILE_H */
