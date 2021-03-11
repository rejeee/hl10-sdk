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

struct mcu_rst_t {
    uint8_t u8Por5V     :1;
    uint8_t u8Por1_5V   :1;
    uint8_t u8Lvd       :1;
    uint8_t u8Wdt       :1;
    uint8_t u8Pca       :1;
    uint8_t u8Lockup    :1;
    uint8_t u8Sysreq    :1;
    uint8_t u8RSTB      :1;
};

struct global_param_t {
    dev_param_t         dev;
    osThreadId          mainid;     /**> device main thread */
    osThreadId          appid;      /**> device app thread for customize use */
    osMutexId           mutex;      /**> device global mutex for customize use */
    uint32_t            dtime;      /**> device time */
    struct mcu_rst_t    rst;        /**> device reset cause   */
    volatile uint8_t    mode;
    volatile uint8_t    aswitch;
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
#ifndef USE_NO_LPWAN
int DevLgwRecv(uint8_t param, struct mac_lorawan_t *ptr);
#endif

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
