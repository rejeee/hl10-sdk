/*******************************************************************************
 * @file    bsp_watchdog.h
 * @brief   Device watchdog interface.
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#ifndef  BSP_CPU_WATCHDOG_H
#define  BSP_CPU_WATCHDOG_H

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>

/**
 * @brief Initialize watchdog
 * @param secs  watchdog timeout
 *
 * @note:   KL1X        Max WDT ~1s
            HC32L13x    Max secs is 26
            STM32F1x    Max secs is 13
            STM32F4x    Max secs is 16
 */
void BSP_WatchdogInit(uint32_t secs);

/**
 * @brief Feed watchdog
 */
void BSP_WatchdogFeed(void);

void BSP_WatchdogEnable(uint8_t enable);

#if defined(__cplusplus)
}
#endif /* __cplusplus */

#endif
