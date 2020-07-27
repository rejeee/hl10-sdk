/*******************************************************************************
 * @file    hl9.h
 * @brief   The main function of special device platform
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#ifndef PLATFORM_HL9_H
#define PLATFORM_HL9_H

#include "platform_conf.h"
#include "common/dev.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GPIO_READ       Gpio_GetInputIO

typedef struct {
    uint8_t u8Por5V     :1;
    uint8_t u8Por1_5V   :1;
    uint8_t u8Lvd       :1;
    uint8_t u8Wdt       :1;
    uint8_t u8Pca       :1;
    uint8_t u8Lockup    :1;
    uint8_t u8Sysreq    :1;
    uint8_t u8RSTB      :1;
} mcu_rst_t;

struct global_param_t {
    dev_param_t     dev;
    BSP_OS_THREAD   mainid;
    BSP_OS_THREAD   appid;
    BSP_OS_MUTEX    mutex;
    uint32_t        dtime;
    mcu_rst_t       rst;
    uint8_t         mode;
    uint8_t         aswitch;
};

/****
Global Variable
****/
extern struct global_param_t    gParam;
extern volatile bool            gEnableRadioRx;

/****
Global Functions
****/

/**
 * @brief Parse LGW Recv data which is payload
 *
 * @param   param    Customizing parameters, distinguishing applications
 *                   7th bit is ACK flag
 *
 * @param   ptr     the pointer for all operating parameters
 *
 * @return  1 currently, other: TBD
 */
int DevLgwRecv(uint8_t param, struct mac_lorawan_t *ptr);

/**
 * @brief Send a "OK\r\n" string
 *
 * @param   addr    Node Address
 * @param   seq     packet sequence
 *
 */
void DevLgwAck(uint32_t addr, uint16_t seq);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
