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
* @file xnandpss_onfi.h
*
* This file implements ONFI specific commands which are used to get the
* parameter page information.
*
* The following commands are supported currently.
* 	- Reset
* 	- Read ID
* 	- READ Parameter Page
* 	- Read Status
* 	- Change Read Column
* 	- Get Features
* 	- Set Features
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
#ifndef ONFI_H		/* prevent circular inclusions */
#define ONFI_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/
#include "xnandpss.h"
/************************** Constant Definitions *****************************/
/*
 * Standard ONFI NAND flash commands
 */

/* 
 * Mandatory commands
 */
#define ONFI_CMD_READ1               0x00 /**< Read cmd (1st cycle) */
#define ONFI_CMD_READ2               0x30 /**< Read cmd (2nd cycle) */
#define ONFI_CMD_CHANGE_READ_COLUMN1 0x05
                             /**< Change Read Column cmd, 1st cycle */

#define ONFI_CMD_CHANGE_READ_COLUMN2 0xE0
                             /**< Change Read Column cmd, 2nd cycle */

#define ONFI_CMD_BLOCK_ERASE1        0x60 /**< Block Erase, 1st cycle */
#define ONFI_CMD_BLOCK_ERASE2        0xD0 /**< Block Erase, 2nd cycle */
#define ONFI_CMD_READ_STATUS         0x70 /**< Read status cmd */

#define ONFI_CMD_PAGE_PROG1          0x80 /**< Page Program cmd, 1st cycle */
#define ONFI_CMD_PAGE_PROG2          0x10 /**< Page Program cmd, 2nd cycle */

#define ONFI_CMD_CHANGE_WRITE_COLUMN 0x85 /**< Change Write column cmd */
#define ONFI_CMD_READ_ID             0x90 /**< Read ID cmd */

#define ONFI_CMD_READ_PARAM_PAGE     0xEC /**< Read Parameter Page cmd */
#define ONFI_CMD_RESET               0xFF /**< Reset cmd */
#define ONFI_RESET_LOOP_COUNT    0x100000 /**< Wait for device ready */

/* 
 * Optional commands
 */
#define ONFI_CMD_GET_FEATURES			0xEE	/**< ONFI Get features
							  command */
#define ONFI_CMD_SET_FEATURES			0xEF	/**< ONFI Set features
							  command */

/* 
 * ONFI Status Register bit offsets
 */
#define ONFI_STATUS_FAIL   0x01 /**< FAIL */
#define ONFI_STATUS_RDY    0x40	/**< READY */

/* 
 * ONFI constants
 */
#define ONFI_ID_LEN           4 /**< ONFI ID Length */
#define ONFI_CRC_INIT    0x4F4E /**< CRC16 Inititialization constant */
#define ONFI_CRC_POLYNOM 0x8005 /**< CRC16 polynomial */
#define ONFI_CRC_ORDER       16 /**< CRC16 order */
#define ONFI_PARAM_PAGE_LEN 256 /**< Parameter page length */
#define ONFI_CRC_LEN        254 /**< CRC16 length */


/**
 * This enum defines the onfi commands.
 */
enum OnfiCommandsEnum {
	READ=0,			/**< ONFI Read */
	CHANGE_READ_COLUMN,	/**< ONFI Change Read Column */
	BLOCK_ERASE,		/**< ONFI Block Erase */
	READ_STATUS,		/**< ONFI Read Status */
	PAGE_PROGRAM,		/**< ONFI Page Program */
	CHANGE_WRITE_COLUMN,	/**< ONFI Change Write Column */
	READ_ID,		/**< ONFI Read ID */
	READ_PARAM_PAGE,	/**< ONFI Read Parameter Page */
	RESET,			/**< ONFI Reset */
	GET_FEATURES,		/**< ONFI Get Features */
	SET_FEATURES		/**< ONFI Set Features */
};


/**************************** Type Definitions *******************************/
/**
 * ONFI 1.0 support
 */
/* 
 * Parameter page structure of ONFI 1.0 specification.
 * Enhanced this sturcture to include ONFI 2.3 information for EZ NAND support.
 */
typedef struct {
	/*
	 * Revision information and features block
	 */
	u8 Signature[4];  /**< Parameter page signature */
	u16 Revision;     /**< Revision Number */
	u16 Features;     /**< Features supported */
	u16 OptionalCmds; /**< Optional commands supported */
	u8 Reserved0[2];  /**< ONFI 2.3: Reserved */
	u16 ExtParamPageLen;  /**< ONFI 2.3: extended parameter page length */
	u8 NumOfParamPages;   /**< ONFI 2.3: No of parameter pages */
	u8 Reserved1[17];     /**< Reserved */
	/*
	 * Manufacturer information block
	 */
	u8 DeviceManufacturer[12]; /**< Device manufacturer */
	u8 DeviceModel[20];        /**< Device model */
	u8 JedecManufacturerId;    /**< JEDEC Manufacturer ID */
	u8 DateCode[2];            /**< Date code */
	u8 Reserved2[13];          /**< Reserved */
	/*
	 * Memory organization block
	*/
	u32 BytesPerPage;          /**< Number of data bytes per page */
	u16 SpareBytesPerPage;     /**< Number of spare bytes per page */
	u32 BytesPerPartialPage;   /**< Number of data bytes per partial page */
	u16 SpareBytesPerPartialPage; /**< Number of spare bytes per partial page*/
	u32 PagesPerBlock;            /**< Number of pages per block */
	u32 BlocksPerLun;             /**< Number of blocks per logical unit(LUN)*/
	u8 NumLuns;                   /**< Number of LUN's */
	u8 AddrCycles;                /**< Number of address cycles */
	u8 BitsPerCell;               /**< Number of bits per cell */
	u16 MaxBadBlocksPerLun;       /**< Bad blocks maximum per LUN */
	u16 BlockEndurance;           /**< Block endurance */
	u8 GuaranteedValidBlock;  /**< Guaranteed valid blocks at beginning */
	u16 BlockEnduranceGvb;    /**< Block endurance for guaranteed valid blocks*/
	u8 ProgramsPerPage;       /**< Number of programs per page */
	u8 PartialProgAttr;       /**< Partial programming attributes */
	u8 EccBits;               /**< Number of bits ECC correctability */
	u8 InterleavedAddrBits;   /**< Number of interleaved address bits */
	u8 InterleavedOperation;  /**< Interleaved operation attributes */
	u8 EzNandSupport;         /**< ONFI 2.3: EZ NAND support parameters */
	u8 Reserved3[12];         /**< Reserved */
	/*
	 * Electrical parameters block
	*/
	u8 IOPinCapacitance;      /**< I/O pin capacitance */
	u16 TimingMode;           /**< Timing mode support */
	u16 PagecacheTimingMode;  /**< Program cache timing mode */
	u16 TProg;                /**< Maximum page program time */
	u16 TBers;                /**< Maximum block erase time */
	u16 TR;                   /**< Maximum page read time */
	u16 TCcs;                 /**< Maximum change column setup time */
	u16 SynTimingMode;   /**< ONFI 2.3: Source synchronous timing mode support*/
	u8 SynFeatures;      /**< ONFI 2.3: Source synchronous features */
	u16 ClkInputPinCap;  /**< ONFI 2.3: CLK input pin capacitance */
	u16 IOPinCap;        /**< ONFI 2.3: I/O pin capacitance */
	u16 InputPinCap;     /**< ONFI 2.3: Input pin capacitance typical */
	u8 InputPinCapMax;   /**< ONFI 2.3: Input pin capacitance maximum */
	u8 DrvStrength;      /**< ONFI 2.3: Driver strength	support */
	u16 TMr;             /**< ONFI 2.3: Maximum multi-plane read time */
	u16 TAdl;   /**< ONFI 2.3: Program page register clear enhancement value */
	u16 TEr;    /**< ONFI 2.3: Typical page read time for EZ NAND */
	u8 Reserved4[6];     /**< Reserved */
	/*
	 * Vendor block
	*/
	u16 VendorRevisionNum;    /**< Vendor specific revision number */
	u8 VendorSpecific[88];    /**< Vendor specific */
	u16 Crc;                  /**< Integrity CRC */
}__attribute__((packed))OnfiNand_Geometry;

/************************** Function Prototypes ******************************/
u8 Onfi_CmdReadStatus(XNandPss *InstancePtr);
int Onfi_NandInit(XNandPss *InstancePtr);


#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
