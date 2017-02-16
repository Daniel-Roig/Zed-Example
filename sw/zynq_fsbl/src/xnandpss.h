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
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xnandpss.h
*
* This file implements a driver for the NAND flash controller.
*
* <b>Driver Initialization</b>
*
* The function call XNandPss_CfgInitialize() should be called by the application
* before any other function in the driver. The initialization function takes
* device specific data (like device id, instance id, and base address) and
* initializes the XNandPss instance with the device specific data.
*
* <b>Device Geometry</b>
*
* NAND flash device is memory device and it is segmented into areas called
* Logical Unit(s) (LUN) and further in to blocks and pages. A NAND flash device
* can have multiple LUN. LUN is sequential raw of multiple blocks of the same
* size. A block is the smallest erasable unit of data within the Flash array of
* a LUN. The size of each block is based on a power of 2. There is no
* restriction on the number of blocks within the LUN. A block contains a number
* of pages. A page is the smallest addressable unit for read and program
* operations. The arrangement of LUN, blocks, and pages is referred to by this
* module as the part's geometry.
*
* The cells within the part can be programmed from a logic 1 to a logic 0
* and not the other way around. To change a cell back to a logic 1, the
* entire block containing that cell must be erased. When a block is erased
* all bytes contain the value 0xFF. The number of times a block can be
* erased is finite. Eventually the block will wear out and will no longer
* be capable of erasure. As of this writing, the typical flash block can
* be erased 100,000 or more times.
*
* The jobs done by this driver typically are:
*       - 8/16 bit operational mode
*       - Read,operation
*       - Read Spare area operation
*       - Error Check and Correction (ECC)
*
* <b>Read Operation</b>
*
* The read call can be used to read a minimum of zero bytes and maximum of
* entire flash but the condition is that number of bytes must be multiple of
* bytes per page. If the Page Address specified to read is out of flash or if
* the number of bytes specified from the page address exceed flash boundaries
* an error is reported back to the user. The read is blocking in nature in that
* the control is returned back to user only after the read operation is
* completed successfully or an error is reported.
*
* @note
*
* This driver is intended to be RTOS and processor independent. It works with
* physical addresses only. Any needs for dynamic memory management, threads,
* mutual exclusion, virtual memory, cache control, or HW write protection
* management must be satisfied by the layer above this driver.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date        Changes
* ----- ----   ----------  -----------------------------------------------
* 1.00a nm     12/10/2010  First release
* 2.00a nm     05/18/2011  Clear NAND support
* </pre>
*
******************************************************************************/

#ifndef XNANDPSS_H              /* prevent circular inclusions */
#define XNANDPSS_H              /* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "xstatus.h"
#include "xnandpss_hw.h"

/************************** Constant Definitions *****************************/
#define XNANDPSS_MAX_TARGETS            1       /**< Max number of targets
                                                  supported */
#define XNANDPSS_MAX_BLOCKS             32768   /**< Max number of Blocks */
#define XNANDPSS_MAX_PAGE_SIZE          16384   /**< Max page size of NAND
                                                  flash */
#define XNANDPSS_MAX_SPARE_SIZE         512     /**< Max spare bytes of a NAND
                                                  flash page */
#define XNANDPSS_ECC_BLOCK_SIZE         512     /**< ECC block size */
#define XNANDPSS_ECC_BYTES              3       /**< ECC bytes per ECC block */

/* Used by SMC ECC */
#define XNANDPSS_PAGE_SIZE_512          512     /**< Page size 512 */
#define XNANDPSS_PAGE_SIZE_1024         1024    /**< Page size 1024 */
#define XNANDPSS_PAGE_SIZE_2048         2048    /**< Page size 2048 */
#define XNANDPSS_PAGE_SIZE_4096         4096    /**< Page size 4096 */
#define XNANDPSS_PAGE_SIZE_8192         8192    /**< Page size 8192 */

#define XNANDPSS_SPARE_SIZE_8           8       /**< Spare bytes size 8 */
#define XNANDPSS_SPARE_SIZE_16          16      /**< Spare bytes size 16 */
#define XNANDPSS_SPARE_SIZE_32          32      /**< Spare bytes size 32 */
#define XNANDPSS_SPARE_SIZE_64          64      /**< Spare bytes size 64 */
#define XNANDPSS_SPARE_SIZE_128         128     /**< Spare bytes size 128 */
#define XNANDPSS_SPARE_SIZE_256         256     /**< Spare bytes size 256 */

#define XNANDPSS_FLASH_WIDTH_8          8       /**< NAND Flash width 8-bit */
#define XNANDPSS_FLASH_WIDTH_16         16      /**< NAND Flash width 16-bit */

/* Macros used for framing SMC AXI command phase and Data phase address */
#define XNANDPSS_END_CMD_NONE           0       /**< No End command */
#define XNANDPSS_END_CMD_INVALID        0       /**< End command invalid */
#define XNANDPSS_CMD_PHASE              1       /**< End command in command
                                                  phase */
#define XNANDPSS_DATA_PHASE             2       /**< End command in data
                                                  phase */

#define XNANDPSS_PAGE_NOT_VALID         -1      /**< Page is not valid in
                                                  command phase */
#define XNANDPSS_COLUMN_NOT_VALID       -1      /**< Column is not valid in
                                                  command phase */

#define XNANDPSS_AXI_DATA_WIDTH         4       /**< AXI Data width for last
                                                  transaction while reading
                                                  and writing */

/* Bit shift for AXI Command/Data phase address calculation */
#define XNANDPSS_START_CMD_SHIFT        3       /**< Start command shift */
#define XNANDPSS_END_CMD_SHIFT          11      /**< End command shift */
#define XNANDPSS_END_CMD_VALID_SHIFT    20      /**< End command valid shift */
#define XNANDPSS_ADDR_CYCLES_SHIFT      21      /**< Address cycles shift */
#define XNANDPSS_CHIP_ADDR_SHIFT        24      /**< Chip address shift */
#define XNANDPSS_ECC_LAST_SHIFT         10      /**< Ecc last shift */
#define XNANDPSS_CLEAR_CS_SHIFT         21      /**< clear chip select shift */
#define XNANDPSS_COMMAND_PHASE_MASK     0x00000000      /**< Command
                                                         phase mask */
#define XNANDPSS_DATA_PHASE_MASK        0x00080000      /**< Data phase mask */

/* Macros used for correcting ECC errors */
#define XNANDPSS_ECC_CORRECT_BYTE_MASK  0x1FF   /**< ECC error correction byte
                                                  position mask, bits[11:3] of
                                                  error code */
#define XNANDPSS_ECC_CORRECT_BIT_MASK   0x7     /**< ECC error correction bit
                                                  position mask, bits[0:2] of
                                                  error code */

/* Flash memory controller operating parameters */
#define XNANDPSS_CLR_CONFIG     \
                        ((XNANDPSS_MEMC_CLR_CONFIG_INT_DISABLE1_MASK)   | \
                        (XNANDPSS_MEMC_CLR_CONFIG_INT_CLR1_MASK)        | \
                        (XNANDPSS_MEMC_CLR_CONFIG_ECC_INT_DISABLE1_MASK))
                        /**< Interrupt settings */

/* Assuming 50MHz clock (20ns cycle time) */
/*
 * These values are taken from ONFI 1.0 specification for Timing Mode 0.
 * T_rc_min = 100ns, T_wc_min = 100ns, T_rea_max = 40ns, T_wp_min = 50ns,
 * T_clr_min = 20ns, T_ar_min = 25ns, T_rr_min = 40ns.
 */
#define XNANDPSS_SET_CYCLES     ((0x2 << XNANDPSS_SET_CYCLES_SET_T6_SHIFT) | \
                                (0x2 << XNANDPSS_SET_CYCLES_SET_T5_SHIFT)  | \
                                (0x1 << XNANDPSS_SET_CYCLES_SET_T4_SHIFT)  | \
                                (0x3 << XNANDPSS_SET_CYCLES_SET_T3_SHIFT)  | \
                                (0x2 << XNANDPSS_SET_CYCLES_SET_T2_SHIFT)  | \
                                (0x5 << XNANDPSS_SET_CYCLES_SET_T1_SHIFT)  | \
                                (0x5 << XNANDPSS_SET_CYCLES_SET_T0_SHIFT))
                        /**< Set cycles settings */

#define XNANDPSS_DIRECT_CMD \
                        ((0x4 << XNANDPSS_DIRECT_CMD_CHIP_SELECT_SHIFT) | \
                        (0x2 << XNANDPSS_DIRECT_CMD_CMD_TYPE_SHIFT))
                        /**< Direct command settings */

#define XNANDPSS_ECC_MEMCFG \
                        ((0x1 << XNANDPSS_ECC_MEMCFG_ECC_MODE_SHIFT)     | \
                        (0x1 << XNANDPSS_ECC_MEMCFG_ECC_READ_END_SHIFT)  | \
                        (0x0 << XNANDPSS_ECC_MEMCFG_ECC_JUMP_SHIFT))
                        /**< ECC memory configuration settings */

#define XNANDPSS_ECC_CMD1 \
                        ((0x80 << XNANDPSS_ECC_MEMCOMMAND1_WR_CMD_SHIFT) | \
                (0x00 << XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_SHIFT)          | \
                (0x30 << XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_END_SHIFT)      | \
                (0x1 << XNANDPSS_ECC_MEMCOMMAND1_RD_CMD_END_VALID_SHIFT))
                        /**< ECC command 1 settings */

#define XNANDPSS_ECC_CMD2       \
                ((0x85 << XNANDPSS_ECC_MEMCOMMAND2_WR_COL_CHANGE_SHIFT)    | \
                (0x05 << XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_SHIFT)     | \
                (0xE0 << XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_END_SHIFT) | \
        (0x1 << XNANDPSS_ECC_MEMCOMMAND2_RD_COL_CHANGE_END_VALID_SHIFT))
                        /**< ECC command 2 settings */

#define XNANDPSS_CLR_CS         (0x1 << XNANDPSS_CLEAR_CS_SHIFT)
                                /**< set Clear chip select */
#define XNANDPSS_ECC_LAST       (0x1 << XNANDPSS_ECC_LAST_SHIFT)
                                /**< set Ecc last */

/**************************** Type Definitions *******************************/
/*
 * This enum contains ECC Mode
 */
typedef enum {
        XNANDPSS_ECC_NONE = 0,  /**< No ECC */
        XNANDPSS_ECC_SW,        /**< Software ECC */
        XNANDPSS_ECC_HW,        /**< Hardware controller ECC */
        XNANDPSS_ECC_ONDIE,     /**< On-Die ECC */
        XNANDPSS_ECC_FLASH      /**< Flash Internal ECC */
} XNandPss_EccMode;

/**
 * This typedef contains configuration information for the flash device.
 */
typedef struct {
        u16  DeviceId;          /**< Instance ID of device */
        u32  SmcBase;           /**< SMC Base address */
        u32  FlashBase;         /**< NAND base address */
        u32  FlashWidth;        /**< Flash width */
} XNandPss_Config;

/**
 * Flash geometry
 */
typedef struct {
	u32 BytesPerPage;       /**< Bytes per page */
	u16 SpareBytesPerPage;   /**< Size of spare area in bytes */
	u32 PagesPerBlock;       /**< Pages per block */
	u32 BlocksPerLun;       /**< Bocks per LUN */
	u8 NumLun;              /**< Total number of LUN */
	u8 FlashWidth;          /**< Data width of flash device */
	u64 NumPages;           /**< Total number of pages in device */
	u64 NumBlocks;          /**< Total number of blocks in device */
	u64 BlockSize;		/**< Size of a block in bytes */
	u64 DeviceSize;         /**< Total device size in bytes */
	u8 RowAddrCycles;	/**< Row address cycles */
	u8 ColAddrCycles;	/**< Column address cycles */
} XNandPss_Geometry;

/**
 * Bad block table descriptor
 */
typedef struct {
        u32 PageOffset;         /**< Page offset where BBT resides */
        u32 SigOffset;          /**< Signature offset in Spare area */
        u32 VerOffset;          /**< Offset of BBT version */
        u32 SigLength;          /**< Length of the signature */
        u32 MaxBlocks;          /**< Max blocks to search for BBT */
        char Signature[4];      /**< BBT signature */
        u8 Version;             /**< BBT version */
        u32 Valid;              /**< BBT descriptor is valid or not */
} XNandPss_BbtDesc;

/**
 * Bad block pattern
 */
typedef struct {
        u32 Options;            /**< Options to search the bad block pattern */
        u32 Offset;             /**< Offset to search for specified pattern */
        u32 Length;             /**< Number of bytes to check the pattern */
        u8 Pattern[2];          /**< Pattern format to search for */
} XNandPss_BadBlockPattern;

/**
 * ECC configuration structure.
 * Contains information related to ECC.
 */
typedef struct {
        u32 NumSteps;           /**< Number of ECC steps for the flash page */
        u32 BlockSize;          /**< ECC block size */
        u32 BytesPerBlock;      /**< Number of ECC bytes for a block */
        u32 TotalBytes;         /**< Total number of ECC bytes for Page */
        u32 EccPos[64];         /**< ECC position in the spare area */
} XNandPss_EccConfig;

/**
 * The XNandPss driver instance data. The user is required to allocate a
 * variable of this type for every flash device in the system. A pointer to a
 * variable of this type is then passed to the driver API functions.
 */
typedef struct XNandPssTag {
        u32 IsReady;                    /**< Device is initialized and ready */
        XNandPss_Config Config;         /**< XNandPss_Config of current
                                          device */
        XNandPss_Geometry Geometry;     /**< Part geometry */
        u32 CommandPhaseAddr;           /**< NAND command phase address */
        u32 DataPhaseAddr;              /**< NAND Data phase address */
        XNandPss_EccConfig EccConfig;   /**< ECC configuration parameters */
        /* Bad block table definitions */
        XNandPss_BbtDesc BbtDesc;       /**< Bad block table descriptor */
        XNandPss_BbtDesc BbtMirrorDesc; /**< Mirror BBT descriptor */
        XNandPss_BadBlockPattern BbPattern;     /**< Bad block pattern to
                                                  search */
        u8 Bbt[XNANDPSS_MAX_BLOCKS >> 2];       /**< Bad block table array */
        u8 DataBuf[XNANDPSS_MAX_PAGE_SIZE + XNANDPSS_MAX_SPARE_SIZE];
                        /**< Data buffer for partial read/writes */
        u8 *SpareBufPtr;                /**< Pointer to store spare buffer */
        u8 EccCalc[XNANDPSS_MAX_SPARE_SIZE];    /**< Buffer for calculated
                                                  ECC */
        u8 EccCode[XNANDPSS_MAX_SPARE_SIZE];    /**< Buffer for stored ECC */
        XNandPss_EccMode EccMode;               /**< ECC Mode */
        int (*ReadPage) (struct XNandPssTag *InstancePtr, u32 Page, u32 Col, u8
                *DstPtr);                       /**< Read Page routine */
} XNandPss;

/**
 * NAND Command format structures
 */
typedef struct {
        int StartCmd;   /**< Start command */
        int EndCmd;     /**< End command */
        u8 AddrCycles;  /**< Number of address cycles */
        u8 EndCmdValid; /**< End command valid */
} XNandPss_CommandFormat;
/***************** Macros (Inline Functions) Definitions *********************/

/**
 * OneHot is used to check if one and only one bit is set.
 * This Macro returns 1 if the value passed is OneHot.
 */
#define OneHot(Value)   (!((Value) & (Value - 1)))

/************************** Function Prototypes ******************************/

extern void *(memset_rom)(void *s, int c, u32 n);
extern void *(memcpy_rom)(void * s1, const void * s2, u32 n);
extern char *strcpy_rom(char *dest, const char *src);

/*
 * Functions in xnand_sinit.c
 */
XNandPss_Config *XNandPss_LookupConfig(u16 DeviceId);

/*
 * Functions in xnand.c
 */
/*
 * Initialization, read, Functions.
 */
int XNandPss_CfgInitialize(XNandPss *InstancePtr, XNandPss_Config *ConfigPtr,
                u32 SmcBaseAddr, u32 FlashBaseAddr);
int XNandPss_Read(XNandPss *InstancePtr, u64 Offset, u32 Bytes, void *DestPtr);
#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
