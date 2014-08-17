#ifndef DEVICE_H
#define DEVICE_H

typedef enum {
	DUMMY = 0,
	HACKRF = 1,
	UHD = 2
} device_type;

typedef enum {
        MODE_OFF = 0,
        MODE_RX = 1,
        MODE_TX = 2,
        MODE_SS = 3
} device_mode;

typedef struct {
	void* driver;
	device_type type;
	device_mode mode;
	uint64_t freq;		/* Hz */
	uint32_t rate;		/* Hz */
} device_t;

typedef struct {
	int (*config)(device_t*);
	int (*xfer)(device_t*, const uint64_t, const uint64_t);
} device_iface_t;

extern device_iface_t devices[];

#endif /* DEVICE_H */

