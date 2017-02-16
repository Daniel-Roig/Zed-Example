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
* @file fsbl.h
*
* Contains the function prototypes, defines and macros for the
* First Stage Boot Loader (FSBL) functionality
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a  jz  03/04/11   Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

#ifndef XIL_FSBL_H
#define XIL_FSBL_H

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_io.h"
#include "xparameters.h"
#include "xpseudo_asm.h"
#include "xil_printf.h"
#include "xuartpss.h"
#include "pcap.h"

/************************** Constant Definitions *****************************/

#define WORD_LENGTH_SHIFT   2

/* The following are the Boot Mode States used for error and status output */

#define SUCCESSFUL_HANDOFF              0x1

/* Error code */
#define ILLEGAL_BOOT_MODE               0xA000
#define ILLEGAL_RETURN                  0xA001
#define EXCEPTION_TAKEN_FAIL            0xA002
#define PERIPHERAL_INIT_FAIL            0xA003
#define DECRYPTION_FAIL                 0xA004
#define BITSTREAM_DOWNLOAD_FAIL         0xA005
#define DMA_TRANSFER_FAIL               0xA006
#define FSBL_HANGS                      0xA007
#define INVALID_FLASH_ADDRESS           0xA008

#define PARTITION_HEADER_CORRUPTION     0xA100
#define IMAGE_HAS_NO_PARTITIONS         0xA101
#define INVALID_PARTITION_LENGTH        0xA102
#define INVALID_PARTITION_HEADER        0xA103
#define EMPTY_PARTITION_HEADER          0xA104
#define FLASH_ENCRYPTION_UNSUPPORTED    0xA105 /**< No decryption on NAND, SD*/
#define SRC_UNALIGNED                   0xA106 /**< must 64-byte aligned */
#define DST_UNALIGNED                   0xA107 /**< must 64-byte aligned */
#define TOO_MANY_PARTITIONS             0xA108
#define ILLEGAL_DECRYPTION              0xA109 /**< no encryption allowed */

/* SLCR Register offsets used by FSBL */
#define BOOT_MODE_REG                   (XPSS_SYS_CTRL_BASEADDR + 0x25C)
#define REBOOT_STATUS_REG               (XPSS_SYS_CTRL_BASEADDR + 0x258)

/* SLCR register used by mio.c */
#define FPGA_RESET_REG                  (XPSS_SYS_CTRL_BASEADDR + 0x240)

/* PSS reset control register define */
#define PSS_RST_CTRL_REG        (XPSS_SYS_CTRL_BASEADDR + 0x200)
#define PSS_RST_MASK            0x1         /**< PSS software reset */

/* SLCR BOOT Mode Register defines */
#define BOOT_MODES_MASK             0x00000007   /**< FLASH types */

#define JTAG_MODE		    0x00000000	 /**< JTAG */
#define QSPI_MODE                   0x00000001   /**< QSPI */
#define NOR_FLASH_MODE              0x00000002   /**< NOR */
#define NAND_FLASH_MODE             0x00000004   /**< NAND */
#define SD_MODE                     0x00000005   /**< SD */

/* SD base address, used by mmc.c and check for flash type */
#define XPSS_SD_BASEADDR            XPSS_SDIO0_BASEADDR

/* Encryption type */
#define IMAGE_ENCRYPTED_EFUSE           0xA5C3C5A3
#define IMAGE_ENCRYPTED_BRAM            0x3A5C3C5A
#define IMAGE_ENCRYPTED_NONE            0x0

/* Move image failure flag */
#define MOVE_IMAGE_FAIL                 0xFFFFFFFF

/* Storage devices have fixed addresses */
#define MEDIA_ADDRESS_MASK      0xFF000000

/* IO accessors */ 
#define FsblIn32                Xil_In32
#define FsblOut32               Xil_Out32
/**************************** Type Definitions *******************************/

/************************** Function Prototypes ******************************/

void OutputStatus(u32 State);
void FsblFallback(void);

u32 FsblGetEncryptType(void);
int FsblSetNextPartition(int Num);

/* PatWDT to avoid WDT reset */
void PatWDT(void);

u32 (*FlashRead)(u32);
u32 (*FlashWrite)(u32, u32);


void *(memset_rom)(void *s, int c, u32 n);
void *(memcpy_rom)(void * s1, const void * s2, u32 n);
char *strcpy_rom(char *dest, const char *src);

/************************** Variable Definitions *****************************/
extern int SkipPartition;

/***************** Macros (Inline Functions) Definitions *********************/

#ifdef __cplusplus
}
#endif

#endif  /* end of protection macro */
