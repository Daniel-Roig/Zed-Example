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
*
*******************************************************************************/
/*****************************************************************************/
/**
*
* @file mio.h
*
* This file contains the interface for the MIO functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm  01/10/10 Initial release
* 2.00a jz   06/01/11 Removed unused defines
*
* </pre>
*
* @note
*
*       To unlock the MIO structure:
*       MioPinEnableOut32(XPSS_SYS_CTRL_BASEADDR + 0x08,0x0000DF0D);
*
*       To lock the MIO structure:
*       MioPinEnableOut32(XPSS_SYS_CTRL_BASEADDR + 0x04,0x0000767B);
*
*
******************************************************************************/
#ifndef ___MIO_H___
#define ___MIO_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "top_level.h"

/************************** Constant Definitions *****************************/
#define MIO_REG_OFFSET      (XPSS_SYS_CTRL_BASEADDR + 0x00000700)
#define MIO_MAX_PIN_NUMBER  54

#define MIO_DEFAULT_MASK    0x00001001  /* pull-up & tri-state */

/*      QSPI Defines for MIO connections */
#define MIO_QSPI_0_REG_OFFSET           0x8 /**< pin  0  for qspi*/

#define MIO_QSPI_SEL0_REG_OFFSET        0x0 /**< pin  0 for qspi sel */
#define MIO_QSPI_SEL1_REG_OFFSET        0x4 /**< pin  1 for qspi sel */

#define MIO_QSPI_NUM_PINS               12
#define MIO_QSPI_SINGLE_DEVICE_NUM_PINS 6

#define MIO_ENABLE_QSPI_MASK            0x2 /**< QSPI output */

/* NAND Defines for MIO connections */
#define MIO_NAND_0_REG_OFFSET           0x0 /**< NAND chip select 0 */

#define MIO_NAND_NUM_PINS               24

#define MIO_ENABLE_NAND_MASK            0x10 /**< NAND output */

/* NOR Defines for MIO connections */
/* unlike NAND and QSPI, NOR has a huge Pin footprint */
/* the pins are contigious so only the first pin is listed here */
/* the others are just addressed by indexing */
#define MIO_NOR_0_REG_OFFSET            0x0  /**< SMC chip select 0 */

#define MIO_NOR_NUM_PINS                40

#define MIO_ENABLE_NOR_MASK             0x8  /**< SMC_cs0 output */

/* SDIO 0 Defines for MIO connections */
#define MIO_SDIO_0_REG_OFFSET           0xA0 /**< pin 40 */

#define MIO_SDIO_NUM_PINS               6

#define MIO_ENABLE_SDIO_MASK            0x80 /**< SD0, input/output */


/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

#define MioIn32        Xil_In32
#define MioOut32       Xil_Out32

#define MioUnlock()     MioOut32(XPSS_SYS_CTRL_BASEADDR + 0x08, 0xDF0DDF0D)

#define MioLock()       MioOut32(XPSS_SYS_CTRL_BASEADDR + 0x04, 0x767B767B)

/************************** Function Prototypes ******************************/

u32 InitMio(void);

/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif


#endif /* ___MIO_H___ */

