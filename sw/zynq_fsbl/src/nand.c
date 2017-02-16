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
* @file nand.c
*
* Contains code for the NAND FLASH functionality. Bad Block management
* is simple: skip the bad blocks and keep going.
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

/***************************** Include Files *********************************/

#include "nand.h"
#include "mio.h"
#include "image_mover.h"

/************************** Constant Definitions *****************************/
//#define DUMP_DEVICE_INFO

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

static u32 ReadNand(u32 Address);

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;
extern u32 FlashOffsetAddress;

/* these values are set by the initialization routine, device dependent */


XNandPss *NandInstPtr;
static XNandPss NandInstance; /* XNand Instance. */
static XNandPss_Config Config;

/******************************************************************************/
/**
*
* This function initializes the controller for the NAND FLASH interface.
*
* @param    none
*
* @return   XST_SUCCESS if the controller initializes correctly
*           XST_FAILURE if the controller fails to initializes correctly
*
* @note    none.
*
****************************************************************************/
u32 InitNand(void)
{
	u32 Status;
	XNandPss_Config *ConfigPtr;

	/* set up base address for access */
	FlashReadBaseAddress = XPSS_NAND_BASEADDR;

	/* set up pointers to instance and the config structure */
	NandInstPtr = &NandInstance;
	ConfigPtr = &Config;
	ConfigPtr->DeviceId = 0;
	ConfigPtr->SmcBase = NAND_CONTROLLER_BASE_ADDRESS;
	ConfigPtr->FlashBase = FlashReadBaseAddress;
	ConfigPtr->FlashWidth = XNANDPSS_FLASH_WIDTH_8;

	/* Initialize the driver */
	Status = XNandPss_CfgInitialize(NandInstPtr, ConfigPtr,
	             ConfigPtr->SmcBase,
	             ConfigPtr->FlashBase);

	if (Status != XST_SUCCESS) {
		debug_xil_printf("InitNand: Status = 0x%.8x\r\n", Status);
		return XST_FAILURE;
	}

#ifdef DUMP_DEVICE_INFO
	debug_xil_printf("NAND FLASH test read  = %8.8X\r\n",
	         ReadNand(XPSS_NAND_BASEADDR));
	debug_xil_printf("NandInstance.Geometry.BytesPerPage = "
	         "%8.8X\r\n", NandInstance.Geometry.BytesPerPage);
	debug_xil_printf("NandInstance.Geometry.SpareBytesPerPage = "
	         "%8.8X\r\n", NandInstance.Geometry.SpareBytesPerPage);
	debug_xil_printf("NandInstance.Geometry.NumLun = %8.8X\r\n",
	         NandInstance.Geometry.NumLun);
	debug_xil_printf("NandInstance.Geometry.PagesPerBlock = "
	         "%8.8X\r\n", NandInstance.Geometry.PagesPerBlock);
	debug_xil_printf("NandInstance.Geometry.FlashWidth = "
	         "%8.8X\r\n", NandInstance.Geometry.FlashWidth);
	debug_xil_printf("NandInstance.Geometry.BlocksPerLun = "
	         "%8.8X\r\n", NandInstance.Geometry.BlocksPerLun);
	debug_xil_printf("NandInstance.Geometry.NumPages = "
	         "%8.8X\r\n", NandInstance.Geometry.NumPages);
	debug_xil_printf("NandInstance.Geometry.NumBlocks = "
	         "%8.8X\r\n", NandInstance.Geometry.NumBlocks);
	debug_xil_printf("NandInstance.Geometry.DeviceSize = "
	         "%8.8X\r\n", NandInstance.Geometry.DeviceSize);

#endif /* DUMP_DEVICE_INFO*/

	return XST_SUCCESS;

}

/******************************************************************************/
/**
*
* This function reads the requested data from NAND FLASH interface.
* This function does not handle bad blocks. Return what it is.
*
* @param    Address into the FLASH data space
*
* @return   Data at the provided offset in the FLASH
*
* @note    none.
*
****************************************************************************/
u32 ReadNand(u32 Address)
{
	u32 Status;
	u32 Page;
	u32 *WordPtr;

	debug_xil_printf("ReadNand: Address = 0x%.8x\r\n", Address);

	WordPtr = (u32 *)NandInstance.DataBuf;

	Page = (Address)/(NandInstance.Geometry.BytesPerPage);

	Status = XNandPss_Read(NandInstPtr,
	           (u64)(Page * (NandInstance.Geometry.BytesPerPage)),
	           NandInstance.Geometry.BytesPerPage,
	           NandInstance.DataBuf);

	if (Status != XST_SUCCESS) {
		debug_xil_printf("ReadNand Failed: Status = 0x%.8x\r\n",
		         Status);
		return 0xFFFFFFFF;
	}

	return WordPtr[((Address) & (NandInstance.Geometry.BytesPerPage - 1))/4];
}

/******************************************************************************/
/**
*
* This function provides the NAND FLASH interface for the Simplified header
* functionality. This function handles bad blocks.
*
* The source address is the absolute good address, bad blocks are skipped
* without incrementing the source address.
*
* @param    SourceAddress is address in FLASH data space, absolute good address
*           DestinationAddress is address in OCM data space
*
* @return   XST_SUCCESS if the transfer completes correctly
*           XST_FAILURE if the transfer fails to completes correctly
*
* @note    none.
*
****************************************************************************/
u32 NandAccess( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{
	u32 Status;
	u32 PageSizeMask;
	u32 PageSize;
	u32 BytesPerBlock;
	u32 TempSourceAddress;
	u32 ByteCount = 0;
	u32 TmpAddress = 0;
	int BlockCount = 0;
	int BadBlocks = 0;
	u32 LastBlockCount = 0;

	PageSize = NandInstance.Geometry.BytesPerPage;
	PageSizeMask = PageSize - 1;
	TempSourceAddress = SourceAddress;
	BytesPerBlock = (NandInstance.Geometry.PagesPerBlock * PageSize);

	/* First get bad blocks before the source address */
	while (TmpAddress < SourceAddress) {
		while (XNandPss_IsBlockBad(NandInstPtr, BlockCount)) {
			BlockCount ++;
			BadBlocks ++;
        	}

		TmpAddress += BytesPerBlock;
		BlockCount ++;
	}

	/* Previous loop advanced BlockCount one more too much */
	LastBlockCount = BlockCount - 1;

	/* Now transfer with bad block skipping */
	while (ByteCount < LengthBytes) {
		int TmpBadBlocks = 0;
		int Length;

		TempSourceAddress = SourceAddress + ByteCount +
		                        BadBlocks * BytesPerBlock;
		BlockCount = TempSourceAddress / BytesPerBlock;

		/* If advance to the next block, needs to check bad block */
		if (BlockCount > LastBlockCount) {
			LastBlockCount = BlockCount;
	
			while (XNandPss_IsBlockBad(NandInstPtr, BlockCount)) {

				BlockCount ++;
				TmpBadBlocks ++;
				debug_xil_printf("Found bad block %d: %d\r\n",
				            BlockCount, TmpBadBlocks);
	        	}

			if (TmpBadBlocks) {
				BadBlocks += TmpBadBlocks;
				TempSourceAddress += TmpBadBlocks * BytesPerBlock;
				LastBlockCount += TmpBadBlocks;
			}
		}

		if (LengthBytes == 4) {
			*((u32 *)DestinationAddress) = ReadNand(TempSourceAddress);

			return XST_SUCCESS;
		}

		/* NAND transfer is page-wise */
		Length = PageSize - (TempSourceAddress & PageSizeMask);

		if ((Length + ByteCount) > LengthBytes) {
			Length = LengthBytes - ByteCount;
		}

		Status = XNandPss_Read(NandInstPtr,
		                       (u64)TempSourceAddress,
		                       Length,
		                       (u32 *)(DestinationAddress + ByteCount));

		if (Status != XST_SUCCESS) {
			debug_xil_printf("NandAccess Failed: source %x, "
			                 "count %d, destinationaddr %x, "
			                 "Status = 0x%.8x\r\n",
			                 (u64)TempSourceAddress, Length,
			                 DestinationAddress + ByteCount, Status);
			return Status;
		}

		ByteCount += Length;
	}

	return XST_SUCCESS;
}
