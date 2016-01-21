#!/usr/bin/env python2

import sys

try:
    from cffi import FFI
except ImportError as e:
    print('error: install cffi, sucka!')
    sys.exit(-1)

ffi = FFI()
ffi.set_source('pylibhackrf', '''
#include <libhackrf/hackrf.h>

uint8_t *hackrf_rx_buffer;
int hackrf_rx_buffer_length;

uint8_t *hackrf_tx_buffer;
int hackrf_tx_buffer_length;
''', libraries=['hackrf'])
ffi.cdef('''
enum hackrf_error {
    HACKRF_SUCCESS = 0,
    HACKRF_TRUE = 1,
    HACKRF_ERROR_INVALID_PARAM = -2,
    HACKRF_ERROR_NOT_FOUND = -5,
    HACKRF_ERROR_BUSY = -6,
    HACKRF_ERROR_NO_MEM = -11,
    HACKRF_ERROR_LIBUSB = -1000,
    HACKRF_ERROR_THREAD = -1001,
    HACKRF_ERROR_STREAMING_THREAD_ERR = -1002,
    HACKRF_ERROR_STREAMING_STOPPED = -1003,
    HACKRF_ERROR_STREAMING_EXIT_CALLED = -1004,
    HACKRF_ERROR_OTHER = -9999,
};

enum hackrf_board_id {
    BOARD_ID_JELLYBEAN  = 0,
    BOARD_ID_JAWBREAKER = 1,
    BOARD_ID_HACKRF_ONE = 2,
    BOARD_ID_INVALID = 0xFF,
};

enum hackrf_usb_board_id {
    USB_BOARD_ID_JAWBREAKER = 0x604B,
    USB_BOARD_ID_HACKRF_ONE = 0x6089,
    USB_BOARD_ID_RAD1O = 0xCC15,
    USB_BOARD_ID_INVALID = 0xFFFF,
};

enum rf_path_filter {
    RF_PATH_FILTER_BYPASS = 0,
    RF_PATH_FILTER_LOW_PASS = 1,
    RF_PATH_FILTER_HIGH_PASS = 2,
};

typedef enum {
    TRANSCEIVER_MODE_OFF = 0,
    TRANSCEIVER_MODE_RX = 1,
    TRANSCEIVER_MODE_TX = 2,
    TRANSCEIVER_MODE_SS = 3,
    TRANSCEIVER_MODE_CPLD_UPDATE = 4
} transceiver_mode_t;

typedef struct hackrf_device hackrf_device;

typedef struct {
    hackrf_device* device;
    uint8_t* buffer;
    int buffer_length;
    int valid_length;
    void* rx_ctx;
    void* tx_ctx;
} hackrf_transfer;

extern uint8_t *hackrf_rx_buffer;
extern int hackrf_rx_buffer_length;
extern uint8_t *hackrf_tx_buffer;
extern int hackrf_tx_buffer_length;

extern "Python" int rx_callback(hackrf_transfer *transfer);
extern "Python" int tx_callback(hackrf_transfer *transfer);

typedef struct {
    uint32_t part_id[2];
    uint32_t serial_no[4];
} read_partid_serialno_t;

struct hackrf_device_list {
    char **serial_numbers;
    enum hackrf_usb_board_id *usb_board_ids;
    int *usb_device_index;
    int devicecount;
    
    void **usb_devices;
    int usb_devicecount;
};
typedef struct hackrf_device_list hackrf_device_list_t;

typedef int (*hackrf_sample_block_cb_fn)(hackrf_transfer* transfer);

int hackrf_init();
int hackrf_exit();
hackrf_device_list_t* hackrf_device_list();
int hackrf_device_list_open(hackrf_device_list_t *list, int idx, hackrf_device** device);
void hackrf_device_list_free(hackrf_device_list_t *list);
int hackrf_open(hackrf_device** device);
int hackrf_open_by_serial(const char* const desired_serial_number, hackrf_device** device);
int hackrf_close(hackrf_device* device);
int hackrf_start_rx(hackrf_device* device, hackrf_sample_block_cb_fn callback, void* rx_ctx);
int hackrf_stop_rx(hackrf_device* device);
int hackrf_start_tx(hackrf_device* device, hackrf_sample_block_cb_fn callback, void* tx_ctx);
int hackrf_stop_tx(hackrf_device* device);
int hackrf_is_streaming(hackrf_device* device);
int hackrf_max2837_read(hackrf_device* device, uint8_t register_number, uint16_t* value);
int hackrf_max2837_write(hackrf_device* device, uint8_t register_number, uint16_t value);
int hackrf_si5351c_read(hackrf_device* device, uint16_t register_number, uint16_t* value);
int hackrf_si5351c_write(hackrf_device* device, uint16_t register_number, uint16_t value);
int hackrf_set_baseband_filter_bandwidth(hackrf_device* device, const uint32_t bandwidth_hz);
int hackrf_rffc5071_read(hackrf_device* device, uint8_t register_number, uint16_t* value);
int hackrf_rffc5071_write(hackrf_device* device, uint8_t register_number, uint16_t value);
int hackrf_spiflash_erase(hackrf_device* device);
int hackrf_spiflash_write(hackrf_device* device, const uint32_t address, const uint16_t length, unsigned char* const data);
int hackrf_spiflash_read(hackrf_device* device, const uint32_t address, const uint16_t length, unsigned char* data);
int hackrf_cpld_write(hackrf_device* device, unsigned char* const data, const unsigned int total_length);
int hackrf_board_id_read(hackrf_device* device, uint8_t* value);
int hackrf_version_string_read(hackrf_device* device, char* version, uint8_t length);
int hackrf_set_freq(hackrf_device* device, const uint64_t freq_hz);
int hackrf_set_freq_explicit(hackrf_device* device, const uint64_t if_freq_hz, const uint64_t lo_freq_hz, const enum rf_path_filter path);
int hackrf_set_sample_rate_manual(hackrf_device* device, const uint32_t freq_hz, const uint32_t divider);
int hackrf_set_sample_rate(hackrf_device* device, const double freq_hz);
int hackrf_set_amp_enable(hackrf_device* device, const uint8_t value);
int hackrf_board_partid_serialno_read(hackrf_device* device, read_partid_serialno_t* read_partid_serialno);
int hackrf_set_lna_gain(hackrf_device* device, uint32_t value);
int hackrf_set_vga_gain(hackrf_device* device, uint32_t value);
int hackrf_set_txvga_gain(hackrf_device* device, uint32_t value);
int hackrf_set_antenna_enable(hackrf_device* device, const uint8_t value);
const char* hackrf_error_name(enum hackrf_error errcode);
const char* hackrf_board_id_name(enum hackrf_board_id board_id);
const char* hackrf_usb_board_id_name(enum hackrf_usb_board_id usb_board_id);
const char* hackrf_filter_path_name(const enum rf_path_filter path);
uint32_t hackrf_compute_baseband_filter_bw_round_down_lt(const uint32_t bandwidth_hz);
uint32_t hackrf_compute_baseband_filter_bw(const uint32_t bandwidth_hz);
''')
ffi.compile()

try:
    import pylibhackrf
except ImportError as e:
    print('error: failed to load libhackrf wrapper!')

# vim: autoindent tabstop=4 shiftwidth=4 expandtab softtabstop=4 filetype=python

