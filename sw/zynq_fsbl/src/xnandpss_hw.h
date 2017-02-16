/******************************************************************************
*
* (c) Copyright 2010 Xilinx, Inc. All rights reserved.
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
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xnandpss_hw.h
*
* This file contains identifiers and low-level macros/functions for the NAND
* Flash controller driver.
* See xnandpss.h for more information.
*
* @note		None
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date    	   Changes
* ----- ----   ----------  -----------------------------------------------
* 1.00a nm     12/10/2010  First release
* </pre>
*
******************************************************************************/

#ifndef XNANDPSS_HW_H		/* prevent circular inclusions */
#define XNANDPSS_HW_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/


/************************** Constant Definitions *****************************/

/* 
 * ECC register offset
 */
#define XNANDPSS_IF0_ECC_OFFSET           0x300 /**< IF0 ECC */
#define XNANDPSS_IF1_ECC_OFFSET           0x400 /**< IF1 ECC */
#define XNANDPSS_ECC_STATUS_OFFSET(addr)  (0x000 + addr) /**< ECC status */
#define XNANDPSS_ECC_MEMCFG_OFFSET(addr)  (0x004 + addr) /**< ECC mem config */
#define XNANDPSS_ECC_MEMCMD1_OFFSET(addr) (0x008 + addr) /**< ECC mem cmd1 */
#define XNANDPSS_ECC_MEMCMD2_OFFSET(addr) (0x00C + addr) /**< ECC mem cmd2 */
#define XNANDPSS_ECC_VALUE0_OFFSET(addr)  (0x018 + addr) /**< ECC value 0 */

/** @name Memory controller status register bit definitions and masks
 *  @{
 */
#define XNANDPSS_MEMC_STATUS_RAW_INT_STATUS1_MASK 0x00000040 /**< IF1 raw IS */
/* @} */


/** @name Clear configuration register bit definitions and masks for
 * interface 1
 *  @{
 */
#define XNANDPSS_MEMC_CLR_CONFIG_INT_DISABLE1_MASK 0x00000002 /**<Disable intr*/
#define XNANDPSS_MEMC_CLR_CONFIG_INT_CLR1_MASK 0x00000010 /**< Clear intr */
#define XNANDPSS_MEMC_CLR_CONFIG_ECC_INT_DISABLE1_MASK 0x00000040 
                                          /**< Disable ECC intr */
/* @} */

/** @name Clear configuration register bit definitions and masks and shift
 *  @{
 */
#define XNANDPSS_DIRECT_CMD_CMD_TYPE_SHIFT    21 /**< Direct cmd shift */
#define XNANDPSS_DIRECT_CMD_CHIP_SELECT_SHIFT 23 /**< Direct cmd CS shift */

/* @} */

/** @name Set cycles register bit definitions and masks and shift
 *  @{
 */

#define XNANDPSS_SET_CYCLES_SET_T0_SHIFT  0  /**< Set cycles set_t0 shift */
#define XNANDPSS_SET_CYCLES_SET_T1_SHIFT  4  /**< Set cycles set_t1 shift */
#define XNANDPSS_SET_CYCLES_SET_T2_SHIFT  8  /**< Set cycles set_t2 shift */
#define XNANDPSS_SET_CYCLES_SET_T3_SHIFT  11 /**< Set cycles set_t3 shift */
#define XNANDPSS_SET_CYCLES_SET_T4_SHIFT  14 /**< Set cycles set_t4 shift */
#define XNANDPSS_SET_CYCLES_SET_T5_SHIFT  17 /**< Set cycles set_t5 shift */
#define XNANDPSS_SET_CYCLES_SET_T6_SHIFT  20 /**< Set cycles set_t6 shift */

#define XNANDPSS_SET_OPMODE_MW_8_BITS  0x0  /**< Opmode mem width for 8 bits*/
#define XNANDPSS_SET_OPMODE_MW_16_BITS 0x1  /**< Opmode mem width for 16 bits*/


/** @name User status register bit definitions and masks
 *  @{
 */
#define XNANDPSS_USER_STATUS_MASK			0x000000FF /**< User
							status mask */
/* @} */

/** @name User config register bit definitions and masks
 *  @{
 */
#define XNANDPSS_USER_CONFIG_MASK			0x000000FF /**< User
							config mask */
/* @} */

/** @name ECC status register bit definitions and masks
 *  @{
 */
#define XNANDPSS_ECC_STATUS_RAW_INT_STATUS_MASK		0x0000003F /**< Ecc
						status raw_int_status mask */
#define XNANDPSS_ECC_STATUS_MASK			0x00000040 /**< Ecc
						status ecc_status mask */
#define XNANDPSS_ECC_LAST_MASK				0x00000180 /**< Ecc
						status ecc_last mask */
#define XNANDPSS_ECC_READ_NOT_WRITE_MASK		0x00000200 /**< Ecc
					status ecc_read_not_write mask */
#define XNANDPSS_ECC_VALID_MASK				0x00007C00 /**< Ecc
						status ecc_valid mask */
#define XNANDPSS_ECC_FAIL_MASK				0x000F8000 /**< Ecc
						status ecc_fail mask */
#define XNANDPSS_ECC_CAN_CORRECT_MASK			0x01F00000 /**< Ecc
					status ecc_can_correct mask */
#define XNANDPSS_ECC_READ_MASK				0x37000000 /**< Ecc
						status ecc_read mask */
/* @} */

/** @name ECC mem config register bit definitions and masks and shifts
 *  @{
 */
#define XNANDPSS_ECC_MEMCFG_ECC_MODE_MASK		0x0000000C
					/**< Ecc cfg ecc_mode mask */

#define XNANDPSS_ECC_MEMCFG_ECC_MODE_SHIFT		2
					/**< Ecc cfg ecc_mode shift */
#define XNANDPSS_ECC_MEMCFG_ECC_READ_END_SHIFT		4
					/**< Ecc cfg ecc_read_end shift */
#define XNANDPSS_ECC_MEMCFG_ECC_JUMP_SHIFT		5
					/**< Ecc cfg ecc_jump shift */

#define XNANDPSS_ECC_MEMCFG_PAGE_SIZE_512		0x1	/**< ECC cfg
					page size value for 512 byte page */
#define XNANDPSS_ECC_MEMCFG_PAGE_SIZE_1024		0x2	/**< ECC cfg
					page size value for 1024 byte page */
#define XNANDPSS_ECC_MEMCFG_PAGE_SIZE_2048		0x3	/**< ECC cfg
					page size value for 2048 byte page */
/* @} */

/** @name ECC mem command1 register bit definitions and masks and shifts
 *  @{
 */
#define XNANDPSS_ECC_MEMCOMMAND1_WR_CMD_SHIFT		0
					/**< Ecc command 1 nand_wr_cmd shift */
#define XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_SHIFT		8
					/**< Ecc command 1 nand_rd_cmd shift */
#define XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_END_SHIFT	16
				/**< Ecc command 1 nand_rd_cmd_end shift */
#define XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_END_VALID_SHIFT	24
			/**< Ecc command 1 nand_rd_cmd_end_valid shift */
/* @} */

/** @name ECC mem command2 register bit definitions and masks
 *  @{
 */
#define XNANDPSS_ECC_MEMCOMMAND2_WR_COL_CHANGE_SHIFT		0
				/**< Ecc command2 nand_wr_col_change shift */
#define XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_SHIFT		8
				/**< Ecc command2 nand_rd_col_change shift */
#define XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_END_SHIFT	16
			/**< Ecc command2 nand_rd_col_change_end shift */
#define XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_END_VALID_SHIFT	24
			/**< Ecc command2 nand_rd_col_change_end_valid shift */
/* @} */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/
#define XNandPss_ReadReg Xil_In32	/**< XNandPss Register register */
#define XNandPss_WriteReg Xil_Out32	/**< XNandPss register write */

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
