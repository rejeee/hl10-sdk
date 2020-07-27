/*******************************************************************************
* Copyright (C) 2019, Huada Semiconductor Co.,Ltd All rights reserved.
*
* This software is owned and published by:
* Huada Semiconductor Co.,Ltd ("HDSC").
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with HDSC
* components. This software is licensed by HDSC to be adapted only
* for use in systems utilizing HDSC components. HDSC shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. HDSC is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* Disclaimer:
* HDSC MAKES NO WARRANTY, EXPRESS OR IMPLIED, ARISING BY LAW OR OTHERWISE,
* REGARDING THE SOFTWARE (INCLUDING ANY ACOOMPANYING WRITTEN MATERIALS),
* ITS PERFORMANCE OR SUITABILITY FOR YOUR INTENDED USE, INCLUDING,
* WITHOUT LIMITATION, THE IMPLIED WARRANTY OF MERCHANTABILITY, THE IMPLIED
* WARRANTY OF FITNESS FOR A PARTICULAR PURPOSE OR USE, AND THE IMPLIED
* WARRANTY OF NONINFRINGEMENT.
* HDSC SHALL HAVE NO LIABILITY (WHETHER IN CONTRACT, WARRANTY, TORT,
* NEGLIGENCE OR OTHERWISE) FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT
* LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS, BUSINESS INTERRUPTION,
* LOSS OF BUSINESS INFORMATION, OR OTHER PECUNIARY LOSS) ARISING FROM USE OR
* INABILITY TO USE THE SOFTWARE, INCLUDING, WITHOUT LIMITATION, ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL OR CONSEQUENTIAL DAMAGES OR LOSS OF DATA,
* SAVINGS OR PROFITS,
* EVEN IF Disclaimer HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* YOU ASSUME ALL RESPONSIBILITIES FOR SELECTION OF THE SOFTWARE TO ACHIEVE YOUR
* INTENDED RESULTS, AND FOR THE INSTALLATION OF, USE OF, AND RESULTS OBTAINED
* FROM, THE SOFTWARE.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/******************************************************************************/
/** \file system_hc32l13x.c
 **
 ** System clock initialization.
 ** @link SampleGroup Some description @endlink
 **
 **   - 2019-03-01  1.0  Lux First version.
 **
 ******************************************************************************/

/******************************************************************************/
/* Include files                                                              */
/******************************************************************************/
#include "base_types.h"
#include "hc32l13x.h"
#include "system_hc32l13x.h"
#include "sysctrl.h"
#include "flash.h"

/**
 ******************************************************************************
 ** System Clock Frequency (Core Clock) Variable according CMSIS
 ******************************************************************************/
uint32_t SystemCoreClock = 4000000;


//add clock source.
void SystemCoreClockUpdate (void) // Update SystemCoreClock variable
{
    SystemCoreClock = Sysctrl_GetHClkFreq();
}

/**
 ******************************************************************************
 ** \brief  对MCU未引出IO端口进行默认配置.
 **
 ** \param  none
 ** \return none
 ******************************************************************************/
static void _HidePinInit(void)
{
    uint32_t tmpReg = M0P_SYSCTRL->PERI_CLKEN;

	M0P_SYSCTRL->PERI_CLKEN_f.GPIO = 1;

#if defined(HC32L13xKxxx)	      //64PIN MCU
	  ///<
#elif defined(HC32L13xJxxx)       //48PIN MCU
	M0P_GPIO->PCADS &= 0xE000;    ///< PC00~PC12配置为数字端口
	M0P_GPIO->PDADS &= 0xFFCB;    ///< PD02/PD04/PD05配置为数字端口

    M0P_GPIO->PCDIR	|= 0x1FFF;    ///< PC00~PC12配置为端口输入
	M0P_GPIO->PDDIR	|= 0x0034;    ///< PD02/PD04/PD05配置为端口输入

	M0P_GPIO->PCPU  |= 0x1FFF;    ///< PC00~PC12配置为上拉
	M0P_GPIO->PDPU  |= 0x0034;    ///< PD02/PD04/PD05配置为上拉

#elif defined(HC32L13xFxxx)       //32PIN MCU
	M0P_GPIO->PAADS &= 0xFFF4;    ///< PA00/PA01/PA03配置为数字端口
	M0P_GPIO->PBADS &= 0x08FB;    ///< PB02/PB08/PB09/PB10/PB12/PB13/PB14/PB15配置为数字端口
	M0P_GPIO->PCADS &= 0xC000;    ///< PC00~PC13配置为数字端口
	M0P_GPIO->PDADS &= 0xFF0B;    ///< PD02/PD04/PD05/PD06/PD07配置为数字端口

	M0P_GPIO->PADIR	|= 0x000B;    ///< PA00/PA01/PA03配置为端口输入
	M0P_GPIO->PBDIR	|= 0xF704;    ///< PB02/PB08/PB09/PB10/PB12/PB13/PB14/PB15配置为端口输入
	M0P_GPIO->PCDIR	|= 0x3FFF;    ///< PC00~PC13配置为端口输入
	M0P_GPIO->PDDIR	|= 0x00F4;    ///< PD02/PD04/PD05/PD06/PD07配置为端口输入

	M0P_GPIO->PAPU  |= 0x000B;    ///< PA00/PA01/PA03配置为上拉
	M0P_GPIO->PBPU  |= 0xF704;    ///< PB02/PB08/PB09/PB10/PB12/PB13/PB14/PB15配置为上拉
	M0P_GPIO->PCPU  |= 0x3FFF;    ///< PC00~PC12配置为上拉
	M0P_GPIO->PDPU  |= 0x00F4;    ///< PD02/PD04/PD06配置为上拉

#elif defined(HC32L13xExxx)       //28PIN MCU
	M0P_GPIO->PAADS &= 0x7FFF;    ///< PA15配置为数字端口
	M0P_GPIO->PBADS &= 0x0007;    ///< PB03~15配置为数字端口
	M0P_GPIO->PCADS &= 0xC000;    ///< PC00~PC13配置位数字端口
	M0P_GPIO->PDADS &= 0xFF0B;    ///< PD02/PD04/PD05/PD06/PD07配置位数字端口

    M0P_GPIO->PADIR	|= 0x8000;    ///< PA15配置为端口输入
    M0P_GPIO->PBDIR	|= 0xFFF8;    ///< PB03~15配置为端口输入
    M0P_GPIO->PCDIR	|= 0x3FFF;    ///< PC00~PC13配置为端口输入
    M0P_GPIO->PDDIR	|= 0x00F4;    ///< PD02/PD04/PD05/PD06/PD07配置为端口输入

	M0P_GPIO->PAPU  |= 0x8000;    ///< PA15配置为上拉
	M0P_GPIO->PBPU  |= 0xFFF8;    ///< PB03~15配置为上拉
	M0P_GPIO->PCPU  |= 0x3FFF;    ///< PC00~PC13配置为上拉
	M0P_GPIO->PDPU  |= 0x00F4;	  ///< PD02/PD04/PD05/PD06/PD07配置为上拉

#endif

	  M0P_SYSCTRL->PERI_CLKEN = tmpReg;
}

/**
 ******************************************************************************
 ** \brief  Setup the microcontroller system. Initialize the System and update
 ** the SystemCoreClock variable.
 **
 ** \param  none
 ** \return none
 ******************************************************************************/
void SystemInit(void)
{
    stc_sysctrl_clk_cfg_t stcCfg;
    /* HCLK  = SYSCLK/1  */
    stcCfg.enHClkDiv  = SysctrlHclkDiv1;
    /*  PCLK = HCLK/1    */
    stcCfg.enPClkDiv  = SysctrlPclkDiv1;

    stcCfg.enClkSrc   = SysctrlClkRCH;

    Flash_WaitCycle(FlashWaitCycle1);

    /* default loader 4MHz Trimming value */
    /* Sysctrl_SetRCHTrim(SysctrlRchFreq4MHz); */
    Sysctrl_SetRCHTrim(SysctrlRchFreq24MHz);    /* load trim from flash */

    Sysctrl_ClkInit(&stcCfg);

    SystemCoreClockUpdate();
	_HidePinInit();
}

