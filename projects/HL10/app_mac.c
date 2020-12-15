/*******************************************************************************
 * @file    app_mac.c
 * @brief   Radio MAC task
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#include "app_mac.h"
#include "at/at_config.h"
#include "mac/node/mac_radio.h"

/****
Global Variables
****/

/****
Local Variables
****/
static void MacTaskHandler(void const *p_arg);

/* Task variables  */
osThreadDef(MacTaskHandler, osPriorityNormal, 1, 0);
#define APP_MAC_NAME   osThread(MacTaskHandler)

static struct mac_lorawan_t sMacParam = {0};

static volatile uint32_t sOnAirMs = 0;

/****
Local Functions
****/

/* print recv data */
static void RadioPrintRecv(bool format)
{
    uint8_t i = 0;

    osSaveCritical();
    osEnterCritical();

    if(sMacParam.ack){
        sMacParam.ack = false;
        printk("\r\nOK\r\n");
    } else if(sMacParam.rxLen > 0){
        if(format){
            printk("\r\n+DATA:%d,%u,%d,%d,%u\r\n\r\n",
                   sMacParam.qos.freqerr, gDevFlash.config.rxfreq,
                   sMacParam.qos.snr, sMacParam.qos.rssi,
                   sMacParam.rxLen);
            for(i = sMacParam.rxIdx; i < sMacParam.rxLen + sMacParam.rxIdx; i++){
                printk("%02X", sMacParam.payload[i]);
            }
            printk("\r\n");
        } else {
            if((gDevFlash.config.dtype >> TYPE_BITS_RAW)&0x01){
                UserDebugWrite(sMacParam.payload, sMacParam.size);
            } else {
                UserDebugWrite(sMacParam.payload + sMacParam.rxIdx, sMacParam.rxLen);
            }
        }
    }
    osExitCritical();
}

/**
 * @brief  MAC task handler
 */
static void MacTaskHandler(void const *p_arg)
{
    uint32_t status = AT_STATUS_NONE;
    uint8_t spiIdx = RADIO_TRX_SPI;

    while (1) {
        if(gDevFlash.config.lcp > 0 && gEnableRadioRx && gDevFlash.config.prop.bdrate <= UART_BRATE_9600){
            if(RadioGetCanRx(spiIdx)){
                /* if you need milliseconds level sleep */
                /* PlatformSleepMs(1000 * gDevFlash.config.lcp); */
                PlatformSleep(gDevFlash.config.lcp);
            }
        }

        if(gEnableRadioRx){
            if(gParam.mode){
                if(RX_MODE_NONE != gDevRam.rx_mode){
                    if(gDevFlash.config.lcp > 0){
                        status = MacRadio_CadProcess(spiIdx, true);
                    } else {
                        status = MacRadio_RxProcess(spiIdx, false);
                    }
                } else {
                    osDelayMs(10);
                }
            } else {
                if(gDevFlash.config.lcp > 0){
                    status = MacRadio_CadProcess(spiIdx, true);
                } else {
                    status = MacRadio_RxProcess(spiIdx, false);
                }
            }

            if(status == AT_STATUS_OK){
                if(gDevFlash.config.lcp <= 0){
                    LED_ON(LED_RF_RX);
                }
                RadioPrintRecv((RX_MODE_FACTORY == gDevRam.rx_mode));
            } else if(status == AT_STATUS_RX_ERR) {
                if(RX_MODE_FACTORY == gDevRam.rx_mode){
                    printk("CRC ERR,SNR:%d, RSSI:%ddBm,Calc:%d\r\n",
                           sMacParam.qos.snr, sMacParam.qos.rssi, sMacParam.qos.freqerr);
                }
            }
            status = AT_STATUS_NONE;
        } else {
            osDelayMs(10);
        }
    }
}

/****
Global Functions
****/
bool AppMacTask(void)
{
    bool success = true;

    memset(&sMacParam, 0 ,sizeof(struct mac_lorawan_t));

    if (false == MacRadio_Init()) {
        LOG_ERR(("Radio Mac Init error.\r\n"));
        return false;
    }

    success = BSP_OS_TaskCreate(&gParam.appid, APP_MAC_NAME, NULL);
    if(false == success){
        LOG_ERR(("Mac task start error.\r\n"));
    }

    return success;
}

bool RadioWaitDone(uint8_t spiIdx, SemIndex_t semIdx, bool tx, uint32_t timeout)
{
    bool success = false;

    /* wait for rxdone or timeout interrupt, timeout is same with tx */
    if(tx){
        sOnAirMs = (timeout > 250)? (timeout - 250) : timeout;
    }
    success = BSP_OS_SemWait(&gIRQSem, timeout);

    return success;
}

bool RadioSemClear(uint8_t spiIdx, SemIndex_t semIdx)
{
    bool success = false;

    success = BSP_OS_SemReset(&gIRQSem);

    return success;
}

uint32_t AT_TxFreq(uint32_t freq, uint8_t *buf, uint32_t len)
{
    uint32_t status = 0;
    uint32_t ticks = rt_time_get();

    sMacParam.freq = freq;
    status = MacRadio_TxProcess(RADIO_TRX_SPI, buf, len, &sMacParam);
    ticks = rt_time_get() - ticks;

    if(ticks < sOnAirMs/2){
        gEnableRadioRx = false;
        AppMacUpdateRx(true);
        RadioInit(RADIO_TRX_SPI);
        gEnableRadioRx = true;
    }

    return status;
}

RadioIrqType_t RadioRxFinish(uint8_t spiIdx)
{
    struct sx12xx_rx_t rxObj;
    RadioIrqType_t type_flag = RADIO_IRQ_UNKOWN;
    {
        rxObj.modem = gDevFlash.config.rps.modem;
        rxObj.bandwidth = gDevFlash.config.rps.bw;
        rxObj.opmode = OP_MODE_RX;
        rxObj.freq = gDevFlash.config.rxfreq;
        rxObj.crc = gDevFlash.config.rps.crc;
    }

    type_flag =  RadioDecode(spiIdx, &rxObj, &sMacParam);

    if(RADIO_IRQ_LORA_RX != type_flag && RADIO_IRQ_FSK_RX != type_flag){
        /* clear buffer */
        memset(sMacParam.payload, 0, sizeof(sMacParam.payload));
        sMacParam.size = 0;
    }

    return type_flag;
}

void RadioCustomization(uint8_t spiIdx, bool tx, RadioSettings_t *ptr)
{
    /**
     * This is a callback function before sending or receiving.
     *
     * You can modify the RF parameters dynamic adjustment of transceiver.
     *
     */

    /* @todo */
    /*
    if(tx){
        ptr->freq = USER_CFG_TFREQ;
    } else {
        ptr->freq = USER_CFG_RFREQ;
    }
    */
}

bool AppMacUpdateRx(bool update)
{
    bool success = MacRadio_AbortRx(RADIO_TRX_SPI);

    return success;
}

void AppMacQueryCSQ(int16_t *rssi, int8_t *snr)
{
    *rssi = sMacParam.qos.rssi;
    *snr = sMacParam.qos.snr;
    return;
}

