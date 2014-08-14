#ifndef DEVICE_H
#define DEVICE_H

typedef enum {
	DUMMY = 0,
	HACKRF = 1
} device_type;

typedef enum {
	NONE = 0
} device_state;

typedef struct {
	void* handle;
	device_type type;
	device_state state;
	uint64_t frequency;	/* Hz */
	uint32_t sample_rate;	/* Hz */
} device_t;

typedef struct {
	int (*init)(device_t*);
	int (*rx)(device_t*);
	int (*tx)(device_t*);
} device_iface_t;

extern device_iface_t devices[];

#endif /* DEVICE_H */

