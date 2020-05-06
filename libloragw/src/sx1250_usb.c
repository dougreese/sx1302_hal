/*
 / _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
 \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 _____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
  (C)2019 Semtech

Description:
    Functions used to handle LoRa concentrator SX1250 radios.

License: Revised BSD License, see LICENSE.TXT file include in the project
*/


/* -------------------------------------------------------------------------- */
/* --- DEPENDANCIES --------------------------------------------------------- */

#include <stdint.h>     /* C99 types */
#include <stdio.h>      /* printf fprintf */
#include <stdlib.h>     /* malloc free */
#include <string.h>

#include "loragw_aux.h"
#include "loragw_mcu.h"
#include "sx1250_usb.h"

/* -------------------------------------------------------------------------- */
/* --- PRIVATE MACROS ------------------------------------------------------- */

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#if DEBUG_RAD == 1
    #define DEBUG_MSG(str)                fprintf(stderr, str)
    #define DEBUG_PRINTF(fmt, args...)    fprintf(stderr,"%s:%d: "fmt, __FUNCTION__, __LINE__, args)
    #define CHECK_NULL(a)                if(a==NULL){fprintf(stderr,"%s:%d: ERROR: NULL POINTER AS ARGUMENT\n", __FUNCTION__, __LINE__);return -1;}
#else
    #define DEBUG_MSG(str)
    #define DEBUG_PRINTF(fmt, args...)
    #define CHECK_NULL(a)                if(a==NULL){return -1;}
#endif

/* -------------------------------------------------------------------------- */
/* --- PRIVATE CONSTANTS ---------------------------------------------------- */

#define WAIT_BUSY_SX1250_MS  1

/* -------------------------------------------------------------------------- */
/* --- PUBLIC FUNCTIONS DEFINITION ------------------------------------------ */

int sx1250_usb_w(void *com_target, uint8_t spi_mux_target, sx1250_op_code_t op_code, uint8_t *data, uint16_t size) {
    int usb_device;
    uint8_t command_size = size + 3;
    uint8_t in_out_buf[command_size];
    int a;
    int i;

    /* check input variables */
    CHECK_NULL(com_target);
    CHECK_NULL(data);

    usb_device = *(int *)com_target;

    /* wait BUSY */
    wait_ms(WAIT_BUSY_SX1250_MS);

    /* prepare frame to be sent */
    in_out_buf[0] = MCU_SPI_TARGET_SX1302; /* MCU -> SX1302 */
    in_out_buf[1] = spi_mux_target; /* SX1302 -> RADIO_A or RADIO_B */
    in_out_buf[2] = (uint8_t)op_code;
    for(i = 0; i < (int)size; i++) {
        in_out_buf[3 + i] = data[i];
    }
    a = mcu_spi_access(usb_device, in_out_buf, command_size);

    /* determine return code */
    if (a != 0) {
        DEBUG_MSG("ERROR: USB SX1250 WRITE FAILURE\n");
        return -1;
    } else {
        DEBUG_MSG("Note: USB SX1250 write success\n");
        return 0;
    }
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

int sx1250_usb_r(void *com_target, uint8_t spi_mux_target, sx1250_op_code_t op_code, uint8_t *data, uint16_t size) {
    int usb_device;
    uint8_t command_size = size + 3;
    uint8_t in_out_buf[command_size];
    int a;
    int i;

    /* check input variables */
    CHECK_NULL(com_target);
    CHECK_NULL(data);

    usb_device = *(int *)com_target;

    /* wait BUSY */
    wait_ms(WAIT_BUSY_SX1250_MS);

    /* prepare frame to be sent */
    in_out_buf[0] = MCU_SPI_TARGET_SX1302; /* MCU -> SX1302 */
    in_out_buf[1] = spi_mux_target; /* SX1302 -> RADIO_A or RADIO_B */
    in_out_buf[2] = (uint8_t)op_code;
    for(i = 0; i < (int)size; i++) {
        in_out_buf[3 + i] = data[i];
    }
    a = mcu_spi_access(usb_device, in_out_buf, command_size);

    /* determine return code */
    if (a != 0) {
        DEBUG_MSG("ERROR: USB SX1250 READ FAILURE\n");
        return -1;
    } else {
        DEBUG_MSG("Note: USB SX1250 read success\n");
        memcpy(data, in_out_buf + 3, size); /* remove the first bytes, keep only the payload */
        return 0;
    }
}

/* --- EOF ------------------------------------------------------------------ */
