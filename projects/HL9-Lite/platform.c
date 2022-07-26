/*******************************************************************************
 * @file    platform.c
 * @brief   The platform interface implementation
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#include "platform/platform.h"
#include <stdarg.h>
#include "at/at_config.h"
#include "radio/sx12xx_common.h"
#include "lptim.h"

#define APP_DEV_VER        1

/****
Global Variables
****/
osSemaphoreDef(debugSemaphore);
#define DEBUG_SEM_NAME      osSemaphore(debugSemaphore)

DeviceFlash_t  gDevFlash;
struct g_at_param_t gDevRam = {
    .sleep_secs = 0,
    .rx_mode = RX_MODE_NONE
};

struct global_param_t gParam = {
    .dev        = {0},
    .mainid     = NULL,
    .appid      = NULL,
    .mutex      = NULL,
    .dtime      = 0,
    .rst        = {0},
    .mode       = 0,
    .aswitch    = 1
};

/**
 * device flash stored values
 */
const uint32_t gDevFlashSize = sizeof(gDevFlash.values)/4;

/****
Local Variables
****/

static BSP_ADC_TypeDef sADCConfig = {
    .ref = AdcMskRefVolSelInBgr1p5,
    .single = 1
};


/* UART user callback */
static void DebugCallback(uint32_t userData)
{
    BSP_LPowerStop();
    /* TODO: */
}

/****
Global Functions
****/
void UserInitGPIO(void)
{
    stc_gpio_cfg_t gpioCfg;

    DDL_ZERO_STRUCT(gpioCfg);

    Sysctrl_SetPeripheralGate(SysctrlPeripheralGpio, TRUE);

    gpioCfg.enDir = GpioDirIn;

    /* Hardware version */
    gpioCfg.enPu = GpioPuEnable;
    gpioCfg.enPd = GpioPdDisable;
    Gpio_Init(UPA_GPIO, UPA_PIN, &gpioCfg);
    osDelayMs(1);
    if(0 == GPIO_READ(UPA_GPIO,UPA_PIN)){
        gPaEnable = true;
        gDevFlash.config.txpow = 0;
        gDevFlash.config.dtype = (0x01 << TYPE_BITS_RFO);
    }
    gpioCfg.enPu = GpioPuDisable;
    gpioCfg.enPd = GpioPdEnable;
    Gpio_Init(UPA_GPIO, UPA_PIN, &gpioCfg);

    /* AT and WKUP */
    Gpio_Init(AT_GPIO, AT_PIN, &gpioCfg);
    Gpio_Init(UKEY_GPIO, UKEY_PIN, &gpioCfg);

    /* IRQ on rising edge trigger */
    EnableNvic(UKEY_PORT_IRQ, IrqLevel3, TRUE);
    Gpio_EnableIrq(UKEY_GPIO, UKEY_PIN, GpioIrqRising);

    /* unused GPIO diabled */
    gpioCfg.enPu = GpioPuDisable;
    gpioCfg.enPd = GpioPdEnable;
    Gpio_Init(UNUSED_GPIO, UNUSED_PIN, &gpioCfg);
    Gpio_Init(GpioPortA, GpioPin2, &gpioCfg);
    Gpio_Init(GpioPortB, GpioPin1, &gpioCfg);

    /* Hardware version */
    Gpio_Init(GpioPortA, GpioPin11, &gpioCfg);
    Gpio_Init(GpioPortB, GpioPin4, &gpioCfg);

    Gpio_Init(GpioPortD, GpioPin0, &gpioCfg);
    Gpio_Init(GpioPortD, GpioPin1, &gpioCfg);
    Gpio_Init(GpioPortD, GpioPin3, &gpioCfg);
    LED_Enable(true);
}

/**
 * @brief IO interrupt service rutine
 */
void PortA_IRQHandler(void)
{
    DevRadioIRQHandler(0);
}

void PortB_IRQHandler(void)
{
    /* User Key IRQ */
    if(TRUE == Gpio_GetIrqStatus(UKEY_GPIO, UKEY_PIN)){
        Gpio_ClearIrq(UKEY_GPIO, UKEY_PIN);
        BSP_LPowerStop();
    }
}

/* This is interrupt handler */
void LpTim_IRQHandler(void)
{
    if (TRUE == Lptim_GetItStatus(M0P_LPTIMER)) {
        Lptim_ClrItStatus(M0P_LPTIMER);
        BSP_LPowerIRQHandler();
    }
}

/**
 * @brief radio hal API implemetation
 *
 * function declarations refer radio common definitions
 */
void RadioDelay(uint32_t ms)
{
    osDelayMs(ms);
}

void RadioLBTLog(uint8_t chan, int rssi)
{
    if(RX_MODE_FACTORY == gDevRam.rx_mode){
        printk("LBT Channel[%u]:%ddBm\r\n", chan, rssi);
    }
}

/**
 * @brief user project special implementation
 * function declarations refer platform common definitions
 */
void UserExternalGPIO(bool enable)
{
    /** @todo: */
}

bool UserDebugInit(bool reinit, uint32_t baudrateType, uint8_t pariType)
{
    BSP_UART_TypeDef uart = {
        .cb = DebugCallback,
        .gpio = DBG_GPIO,
        .tx_pin = DBG_TX_PIN,
        .rx_pin = DBG_RX_PIN,
        .af = DBG_AF,
        .pu = GpioPuEnable,
        .pd = GpioPdDisable,
        .idx = DBG_UART_IDX,
        .bdtype = baudrateType,
        .pri = pariType
    };

    BSP_UartInit(&uart);

    return true;
}

void UserDebugDeInit(void)
{
    BSP_UART_TypeDef uart = {
        .idx = DBG_UART_IDX
    };

    BSP_UartDeInit(&uart);

    return;
}

void UserDebugWrite(const uint8_t *data, uint32_t len)
{
    BSP_UartSend(DBG_UART_IDX, data, len);
}


void DevCfg_UserDefault(uint8_t opts)
{
    if(gPaEnable){
        gDevFlash.config.txpow = 0;
        gDevFlash.config.dtype = (0x01 << TYPE_BITS_RFO);
    }
}

bool DevCfg_UserUpdate(uint8_t *data, uint32_t len)
{
    /** @todo: */
    return true;
}

bool DevUserInit(void)
{
    bool success = false;

    /* init default parameters */
    gParam.dev.ver = APP_DEV_VER;

    success = UserDebugInit(false, UART_BRATE_9600, UART_PARI_NONE);

    DevGetVol(0, 0);

    return success;
}

void DevGetVol(uint32_t param1, uint16_t param2)
{
    uint32_t adc = 0;

    BSP_ADC_Enable(&sADCConfig);
    adc = BSP_ADC_Sample(BSP_ADC1, AdcAVccdiv3Input);
    gParam.dev.vol = (adc*1500*3)/4095;

    /** example */
    /*
    adc = BSP_ADC_Sample(0, AdcExInputCH2);
    adc = (adc*1500)/4095;
    */
    BSP_ADC_Disable(&sADCConfig);

    gParam.dev.level = calc_level(32, MIN_VOL_LEVEL, MAX_VOL_LEVEL, gParam.dev.vol);
    gParam.dev.res = (gParam.dev.vol+50)/100;

    return;
}

/**
 *  NOTE:
 *  User should redefine print function to reduce code size
 *  by disable IAR print Automatic choice of formatter.
 */
int printk(const char *fmt_s, ...)
{
    static char outbuf[128] = {0};

    va_list ap;
    int result;
    uint32_t len = sizeof(outbuf);

    va_start(ap, fmt_s);
    result = vsnprintf(outbuf, len, fmt_s, ap);
    va_end(ap);

    if(result > 0){
        BSP_UartSend(DBG_UART_IDX, (uint8_t *)outbuf, result);
    }

    return result;
}

void BSP_WatchdogInit(uint32_t secs)
{
    /** @todo: */
}

void BSP_WatchdogFeed(void)
{
    /** @todo: */
}

void BSP_WatchdogEnable(uint8_t enable)
{
    /** @todo: */
}
