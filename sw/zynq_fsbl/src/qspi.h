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
* @file qspi.h
*
* This file contains the interface for the QSPI FLASH functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm  01/10/10 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___QSPI_H___
#define ___QSPI_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "top_level.h"

/************************** Constant Definitions *****************************/

#define QSPI_CONFIG_OFFSET       (0x00) /**< Configuration */
#define QSPI_STATUS_OFFSET       (0x04) /**< Interrupt Status  */
#define QSPI_IEN_OFFSET          (0x08) /**< Interrupt Enable  */
#define QSPI_IDIS_OFFSET         (0x0C) /**< Interrupt Disable  */
#define QSPI_IMASK_OFFSET        (0x10) /**< Interrupt Mask  */
#define QSPI_ENABLE_OFFSET       (0x14) /**< Enable  */
#define QSPI_DELAY_OFFSET        (0x18) /**< Delay Register  */
#define QSPI_TXD_00_00_OFFSET    (0x1C)
#define QSPI_RXD_OFFSET          (0x20) /**< Receive Data (RX FIFO)  */
#define QSPI_SIC_OFFSET          (0x24) /**< Slave Idle Count Register */
#define QSPI_TX_THRESH_OFFSET    (0x28) /**< TX FIFO Threshold level */
#define QSPI_RX_THRESH_OFFSET    (0x2C) /**< RX FIFO Threshold level */
#define QSPI_GPIO_OFFSET         (0x30) /**< General Purpose IO */
#define QSPI_PROG_INST_OFFSET    (0x34)
#define QSPI_LPBK_CLK_DLY_OFFSET (0x38) /**< Loopback master clk delay adj */
#define QSPI_TXD_00_01_OFFSET    (0x80) /**< Transmit 1-byte instruction  */
#define QSPI_TXD_00_02_OFFSET    (0x84) /**< Transmit 2-byte instruction */
#define QSPI_TXD_00_03_OFFSET    (0x88) /**< Transmit 3-byte instruction */

/* Linear Adapter Registers */
#define QSPI_LINEAR_CFG_OFFSET   (0xA0) /**< Config reg for Linear Adapter */
#define QSPI_LINEAR_STS_OFFSET   (0xA4) /**< Status reg for Linear Adapter */


#define QSPI_MOD_ID_OFFSET       (0xFC) /**< Module ID Register 0x0109_0101 */

/* Bit Positions for the Quad-SPI controller Enable Register */

#define QSPI_CONTROLLER_ENABLE    (0x00000001)
#define QSPI_CONFIG_RESET_AUTO    (0x800238C1)  /* Auto mode   */
//#define QSPI_CONFIG_RESET_AUTO    (0x800238F9)    /* Auto mode, max divider */
//#define QSPI_CONFIG_RESET_AUTO    0x800238C9    /* divider 4 */


/* Bit postiions for QSPI_LINEAR_CFG_OFFSET */
#define QSPI_LINEAR_CFG_ENABLE_MASK    (0x80000000)
#define QSPI_LINEAR_CFG_TWO_DEV_MASK   (0x40000000)
#define QSPI_LINEAR_CFG_SEP_BUS_MASK   (0x20000000)
#define QSPI_LINEAR_CFG_MERGE_MASK     (0x10000000)
#define QSPI_LINEAR_CFG_INST_CHK_MASK  (0x08000000)
#define QSPI_LINEAR_CFG_MODE_EN_MASK   (0x04000000)
#define QSPI_LINEAR_CFG_CMD_MERGE_MASK (0x02000000)
#define QSPI_LINEAR_CFG_MODE_ON_MASK   (0x01000000)
#define QSPI_LINEAR_CFG_MODE_BITS_MASK (0x00FF0000)

#define QSPI_LINEAR_CFG_ADDR_TYPE_MASK (0x000C0000) /**< Serial address mode */
                                            /**< 2'b00 bit serial address */
                                            /**< 2'b01 2-bit serial address */
                                            /**< 2'b10 4-bit serial address */
                                            /**< 2'b11      not defined */

#define QSPI_LINEAR_CFG_DATA_TYPE_MASK (0x00030000) /**< Serial read mode */

#define QSPI_LINEAR_CFG_DUMMY_CYC_MASK          (0x00000F00)

#define QSPI_LINEAR_CFG_INST_CODE_MASK          (0x000000FF)


/* Bit postiions for QSPI_LINEAR_CFG_OFFSET */

#define QSPI_LINEAR_STS_CMD_MERGED_MASK         (0x00000010)
#define QSPI_LINEAR_STS_FB_RECVD_MASK           (0x00000004)
#define QSPI_LINEAR_STS_WR_RECVD_MASK           (0x00000002)
#define QSPI_LINEAR_STS_UNKN_INST_MASK          (0x00000001)

/* LQSPI related definitons*/
#define QSPI_LINEAR_CONFIG_READ                 (0x80000003)
#define QSPI_LINEAR_CONFIG_DOREAD               (0x8000013B)
#define QSPI_LINEAR_CONFIG_QOREAD               (0x8000016B)    //0x8400216B



/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define QspiIn32        Xil_In32
#define QspiOut32       Xil_Out32

/************************** Function Prototypes ******************************/


u32 InitQspi(void);

u32 QspiAccess( u32 SourceAddress,
                u32 DestinationAddress,
                u32 LengthBytes);

/************************** Variable Definitions *****************************/


#ifdef __cplusplus
}
#endif


#endif /* ___QSPI_H___ */

