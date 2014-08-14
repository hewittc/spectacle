#ifndef DEVICE_H
#define DEVICE_H

typedef enum {
	DUMMY = 0,
	HACKRF = 1
} device_type;

typedef struct device_t device_t;
typedef struct device_iface_t device_iface_t;

struct device_t {
	void* driver;
	device_iface_t* iface;
	device_type type;
	uint64_t freq;		/* Hz */
	uint32_t rate;		/* Hz */
};

struct device_iface_t {
	int (*init)(device_t*);
	int (*rx)(device_t*);
	int (*tx)(device_t*);
};

extern device_iface_t devices[];

#endif /* DEVICE_H */

