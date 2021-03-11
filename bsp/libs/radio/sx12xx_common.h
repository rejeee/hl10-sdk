/*******************************************************************************
 * @file    sx12xx_common.h
 * @brief   The common interface for sx12xx
 *
 * This is a convenience header file for defining difference radio chip.
 *
 * @version 1.0.0
 *******************************************************************************
 * @license Refer License or other description Docs
 * @author  Felix
 ******************************************************************************/
#ifndef SX12XX_COMMON_H
#define SX12XX_COMMON_H

/****
Include Files
****/
#include <stdint.h>
#include "radio/sx12xx_define.h"

/****
Macro definiens
****/

/** @brief 32MHz XTAL frequency */
#define RADIO_XOSC_FREQ         32000000U

/** @brief Radio TX/RX maximal payload */
#define RADIO_MAX_PAYLOAD       255U
#define RADIO_DEFAULT_FREQ      475500000U
#define RADIO_TIMEOUT_MS        200u

#ifdef __cplusplus
extern "C" {
#endif

/****
type definiens
****/

/**
 * @brief Radio IRQ type
 * @enum RadioIrqType_t
 */
typedef enum
{
  RADIO_IRQ_TIMEOUT = 0,
  RADIO_IRQ_LORA_TX,
  RADIO_IRQ_LORA_RX,
  RADIO_IRQ_FSK_TX,
  RADIO_IRQ_FSK_RX,
  RADIO_IRQ_RX_ERR,
  RADIO_IRQ_RX_TIMEOUT,
  RADIO_IRQ_TX_ERR,
  RADIO_IRQ_UNKOWN
} RadioIrqType_t;

/**
 * @brief Radio driver supported modems
 * @enum RadioModems_t
 */
typedef enum
{
  MODEM_FSK = 0,
  MODEM_LORA,
  MODEM_RANGING,
  MODEM_FLRC,
  MODEM_BLE,
  MODEM_NONE = 0x0F
} RadioModems_t;

/**
 * @brief Radio driver operation mode
 * @enum RadioOpMode_t
 */
typedef enum
{
  OP_MODE_STANDBY = 0,
  OP_MODE_TX,
  OP_MODE_RX,
  OP_MODE_RX_SINGLE,
  OP_MODE_CAD
} RadioOpMode_t;

typedef enum
{
  CHIP_1261 = 0,
  CHIP_1262,
  CHIP_1268
} ChipType_t;

enum
{
  PA_HMAX_22DB = 0,
  PA_HMAX_20DB,
  PA_HMAX_17DB,
  PA_HMAX_14DB,
  PA_HMAX_10DB
};

/**
 * @brief Radio Settings
 * @struct RadioSettings_t
 */
typedef struct
{
    LoRaSettings_t      LoRa;
    FskSettings_t       Fsk;
    uint32_t            timeout;        /* rx and tx timeout */
    uint32_t            freq;
    uint32_t            syncword;
    uint32_t            preamb  : 16;   /* RadioOpMode_t */
    uint32_t            crcon   : 1;    /* [0: OFF; 1: ON] */
    uint32_t            opmode  : 3;    /* RadioOpMode_t */
    uint32_t            role    : 1;    /* [0: slave; 1: master] */
    uint32_t            fixlen  : 11;   /* [0: variable; other: fixed length] */
    uint32_t            rngaddr;
    uint8_t             size;
    uint8_t             modem;          /* [0: FSK; 1: LoRa] */
    uint8_t             rfo;            /* rfo */
    int8_t              power;          /* the output power [dbm] */
} RadioSettings_t;

/*
 * Radio Rx Config
 *
 * bandwidth  [0: 7.8  kHz, 1: 10.4 kHz, 2: 15.6 kHz, 3: 20.8 kHz,
 *              4: 31.2 kHz, 5: 41.6 kHz, 6: 62.5 kHz, 7: 125 kHz,
 *              8: 250  kHz, 9: 500  kHz, other: Reserved]
 *
 * rxmode     [false] single rx [true] continuous
 * crc        [false] off [true] on
 */
struct sx12xx_rx_t
{
    uint32_t    freq;
    uint8_t     bandwidth;
    uint8_t     opmode;
    uint8_t     crc;
    uint8_t     modem;
};

/**
 * @brief config options union define
 * @union ConfigOptUnion
 */
typedef union
{
    struct {
        uint8_t cfg     : 1;    /**> [0 ignore radio settings, 1 reconfig radio settings ] */
        uint8_t notx    : 1;    /**> [0 direct send after config, 1 no send after config ] */
        uint8_t errata  : 1;    /**> [0 , 1 with errata ] */
    } bits;
    uint8_t value;
} ConfigOptUnion;

/**
 * @brief Get current HAL version
 */
uint32_t RadioHalVersion(void);

/**
 * @brief Radio Driver Init
 */
bool RadioInit(uint8_t spiIdx);

/**
 * @brief a delay function for radio operatioin
 *
 * @note Need external implemention
 *
 * @param ms  values of delay
 */
void RadioDelay(uint32_t ms);
void RadioDelayUs(uint32_t us);

void RadioReset(uint8_t spiIdx);

void RadioSleep(uint8_t spiIdx);

int8_t RadioMatchPower(uint8_t spiIdx, int8_t power);

ChipType_t RadioMatchChip(uint8_t spiIdx);

/**
 * @brief ANT switch for TX and RX
 *
 * @param On/Off  0--TX, 1--RX
 */
void RadioAntSwitch(uint8_t spiIdx, bool rx);

/**
 * @brief ANT low power set
 *
 * @param status    0--active; 1-deactive(lowpower)
 */
void RadioAntLowPower(uint8_t spiIdx, uint8_t status);

uint16_t RadioReadFlags(uint8_t spiIdx, uint16_t irqmask, bool clear);
uint16_t RadioGetDeviceErrors(uint8_t spiIdx);
void RadioClearDeviceErrors(uint8_t spiIdx);

uint16_t RadioChipVersion(uint8_t spiIdx);

double RadioRangingRaw(uint8_t spiIdx, uint8_t bw, uint8_t rangeType);
double RadioCalculateRange(double rangeRaw[], uint32_t count, uint8_t bw, uint8_t sf, RadioQoS_t *ptr);

#ifdef __cplusplus
}
#endif

#endif
