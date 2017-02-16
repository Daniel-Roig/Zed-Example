/******************************************************************************
*
* (c) Copyright 2011 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
******************************************************************************/
/*****************************************************************************/
/**
* @file xparameters_pss.h
*
* This file contains the address definitions for the hard peripherals
* attached to the Cortex A9 core.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  02/01/09 Initial version,
* </pre>
*
* @note None.
*
******************************************************************************/

#ifndef _XPARAMETERS_PSS_H_
#define _XPARAMETERS_PSS_H_

/***************************** Include Files *********************************/

#ifdef __cplusplus
extern "C" {
#endif
/***************** Macros (Inline Functions) Definitions *********************/
/**************************** Type Definitions *******************************/

/************************** Constant Definitions *****************************/


/*
 * This block contains constant declarations for the peripherals
 * within the hardblock
 */

#define XPSS_PERIPHERAL_BASEADDR                0xE0000000

#define XPSS_UART0_BASEADDR                     0xE0000000
#define XPSS_UART1_BASEADDR                     0xE0001000

#define XPSS_USB0_BASEADDR                      0xE0002000
#define XPSS_USB1_BASEADDR                      0xE0003000

#define XPSS_I2C0_BASEADDR                      0xE0004000
#define XPSS_I2C1_BASEADDR                      0xE0005000

#define XPSS_SPI0_BASEADDR                      0xE0006000
#define XPSS_SPI1_BASEADDR                      0xE0007000

#define XPSS_CAN0_BASEADDR                      0xE0008000
#define XPSS_CAN1_BASEADDR                      0xE0009000

#define XPSS_GPIO_BASEADDR                      0xE000A000

#define XPSS_GEM0_BASEADDR                      0xE000B000
#define XPSS_GEM1_BASEADDR                      0xE000C000

#define XPSS_QSPI_BASEADDR                      0xE000D000
#define XPSS_QSPI_LIN_BASEADDR                  0xFC000000

#define XPSS_SMCC_BASEADDR                      0xE000E000

#define XPSS_SDIO0_BASEADDR                     0xE0100000
#define XPSS_SDIO1_BASEADDR                     0xE0101000

#define XPSS_IOU_BUS_CFG_BASEADDR               0xE0200000

#define XPSS_NAND_BASEADDR                      0xE1000000
#define XPSS_PARPORT0_BASEADDR                  0xE2000000
#define XPSS_PARPORT1_BASEADDR                  0xE4000000

#define XPSS_SYS_CTRL_BASEADDR                  0xF8000000      /**< AKA SLCR */

#define XPSS_TTC0_BASEADDR                      0xF8001000
#define XPSS_TTC1_BASEADDR                      0xF8002000

#define XPSS_DMAC0_BASEADDR                     0xF8003000
#define XPSS_DMAC1_BASEADDR                     0xF8004000

#define XPSS_WDT_BASEADDR                       0xF8005000

#define XPSS_DDR_CTRL_BASEADDR                  0xF8006000

#define XPSS_DEV_CFG_APB_BASEADDR               0xF8007000

#define XPSS_L2CC_BASEADDR                      0xF8F02000

#define XPSS_SCU_BASEADDR                       0xF8F00000

#define XPSS_GIC_CPU_BASEADDR			0xF8F00100
#define XPSS_GIC_DIS_BASEADDR			0xF8F01000

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* protection macro */
