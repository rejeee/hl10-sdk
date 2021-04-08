/*******************************************************************************
 * @file    app_project.c
 * @brief   Project application interface
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#include "app.h"
#include "app_at.h"
#include "app_mac.h"
#include "at/at_config.h"
#include "radio/sx12xx_common.h"

#define TASK_PERIOD_MS      100U    /* unit ms */

/* Code Version */
char *gCodeVers = "1007";

/****
Global Variables
****/
volatile bool   gEnableRadioRx  = true;

/****
Local Variables
****/

/****
Local Functions
****/

/**
 * @brief  Initialize all tasks
 *
 * @return  true if success else false
 */
static bool AppTaskInit(void)
{
    bool result = true;

    APP_FeedDog();
    result = AppMacTask();

    if(result){
        APP_FeedDog();
        result = AppATTask();
    }

    return result;
}

static void AppTaskManager(void)
{
    BSP_OS_MutexLock(&gParam.mutex, OS_ALWAYS_DELAY);
    UserCheckAT();
    BSP_OS_MutexUnLock(&gParam.mutex);
#if 0
    static int count = 0;
    stc_rtc_time_t stcTime;
    if(count++%10 == 0){
        BSP_RTC_GetDateTime(&stcTime);
        printk("%d-%d-%d %d:%d:%d\r\n", stcTime.u8Year + 2000,stcTime.u8Month,stcTime.u8Day,
            stcTime.u8Hour,stcTime.u8Minute,stcTime.u8Second);
    }
#endif
}

/****
Global Functions
****/
bool AppTaskCreate(void)
{
    uint8_t result = RJ_ERR_OK;

    bool success = false;

    /* watchdog timeout 6s refer MCU datasheet */
    System_HidePinInit(HC32L13xFxxx);

    result = PlatformInit(6);

    /* Low Energy Timer and DeepSleep init */
    if(false == BSP_LPowerInit(false)){
        return false;
    }

    success = DevUserInit();
    if(gParam.dev.vol < 2000 && RJ_ERR_OK == result){
        result = RJ_ERR_BAT;
    }

    if(RJ_ERR_OK != result){
        char *errstr = "MCU";
        switch(result){
        case RJ_ERR_OS:
            errstr ="OS";
            break;
        case RJ_ERR_FLASH:
            errstr ="flash";
            break;
        case RJ_ERR_PARAM:
            errstr ="config";
            break;
        case RJ_ERR_CHK:
            errstr ="sign";
            break;
        case RJ_ERR_BAT:
            errstr ="LowBat";
            break;
        }

        printk("LoRa %s Firmware V%s %s error[%u], please recovery\r\n",
               MODULE_NAME, gCodeVers, errstr, gParam.dev.vol);

        if(gDevFlash.config.lcp <= 0){
            osDelayMs(1000);
        } else {
            PlatformSleep(1);
        }

        return false;
    }

    if(false == success){
        return false;
    }

    printk("LoRa %s SDK, HAL V%u:%u, XTL:%d, Firmware V%s\r\n", MODULE_NAME,
           RadioHalVersion(), AT_Version(), gParam.dev.extl, gCodeVers);

    if(success) {
        success = AppTaskInit();
    }

    return success;
}

void AppTaskExtends(void)
{
    while (1) {
        APP_FeedDog();
        AppTaskManager();
        osDelayMs(TASK_PERIOD_MS);
        if(gDevFlash.config.lcp <= 0){
            LED_OFF(LED_RF_RX);
        }
    }
}
