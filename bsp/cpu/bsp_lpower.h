/*******************************************************************************
 * @file    bsp_lowpower.h
 * @brief   The LowPower function of special device platform
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#ifndef BSP_CPU_LOWPOWER_H
#define BSP_CPU_LOWPOWER_H

/****
Include Files
****/
#include <stdbool.h>
#include <stdint.h>
#include "em_device.h"

/**
 * @brief Chip lowpower mode initialization
 *
 * @param extl   whether or not use XTL
 *
 * @return  true if initialize success else false
 */
bool BSP_LPowerInit(bool extl);
void BSP_LPowerIRQHandler(void);

/**
 * @brief Set Chip enter low power mode.
 *
 * @param secs   The chip wake up after delay seconds
 *               if sec is zero, chip sleep always expect wakeup by IRQ.
 *
 * @return  true if timeout is arrived else false by IRQ wakeup
 */
bool BSP_LPowerSleep(const uint32_t secs);

/**
 * @brief Set Chip enter low power mode.
 *
 * @note: this @ms must not exceed WDT timeout
 *
 * @param ms    The chip wake up after delay millisecond
 *              sec is zero, chip immediate return.
 *
 * @return  true if @ms is arrived else false by IRQ wakeup
 */
bool BSP_LPowerSleepMs(const uint32_t ms);

void BSP_LPowerStop(void);
void BSP_LPowerIdle(void);

/**
 * @brief Get seconds which MCU has been running or MCU elpased time
 *
 * @return  same as LPTimer IRQ count before stop.
 */
uint32_t BSP_LPowerGetSecs(void);

#endif
