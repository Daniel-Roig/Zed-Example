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
* @file xnandpss_onfi.c
*
* This module implements the ONFI specific commands. 
* See xnandpss_onfi.h for more information.
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

/***************************** Include Files *********************************/
#include "xnandpss_onfi.h"
#include "top_level.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static void Onfi_ReadData(XNandPss *InstancePtr, u8 *Buf, u32 Length);

static int Onfi_CmdReset(XNandPss *InstancePtr);

static void Onfi_CmdReadId(XNandPss *InstancePtr, u8 Address);

static int Onfi_CmdReadParamPage(XNandPss *InstancePtr);

static unsigned long Onfi_Crc16(u8 *Buf);

static int Onfi_ReadParamPage(XNandPss *InstancePtr, u8 *Buf);

extern void XNandPss_SendCommand(XNandPss *InstancePtr, XNandPss_CommandFormat
		*Command, int Page, int Column);
/************************** Variable Definitions *****************************/

/**
 * This structure defines the onfi command format sent to the flash.
 */
XNandPss_CommandFormat OnfiCommands[] = {
	{ONFI_CMD_READ1, ONFI_CMD_READ2, 5, XNANDPSS_CMD_PHASE},	
					/*<< Read command format */
	{ONFI_CMD_CHANGE_READ_COLUMN1, ONFI_CMD_CHANGE_READ_COLUMN2,
	2, XNANDPSS_CMD_PHASE},		/*<< Change Read column format */
	{ONFI_CMD_BLOCK_ERASE1, ONFI_CMD_BLOCK_ERASE2, 3, XNANDPSS_CMD_PHASE},
					/*<<Block Erase command format */
	{ONFI_CMD_READ_STATUS, XNANDPSS_END_CMD_NONE, 0,
		XNANDPSS_END_CMD_INVALID},
					/*<< Read Status command format */
	{ONFI_CMD_PAGE_PROG1, ONFI_CMD_PAGE_PROG2, 5, XNANDPSS_DATA_PHASE},
					/*<< Page program command format */
	{ONFI_CMD_CHANGE_WRITE_COLUMN, XNANDPSS_END_CMD_NONE, 2,
		XNANDPSS_END_CMD_INVALID},	/*<< Change Write Column
						  command format */
	{ONFI_CMD_READ_ID, XNANDPSS_END_CMD_NONE, 1, XNANDPSS_END_CMD_INVALID},
					/*<< Read ID command format */
	{ONFI_CMD_READ_PARAM_PAGE, XNANDPSS_END_CMD_NONE, 1,
		XNANDPSS_END_CMD_INVALID},
					/*<< Read Param Page command format */
	{ONFI_CMD_RESET, XNANDPSS_END_CMD_NONE, 0, XNANDPSS_END_CMD_INVALID},
					/*<< Reset command format */
	{ONFI_CMD_GET_FEATURES, XNANDPSS_END_CMD_NONE, 1,
		XNANDPSS_END_CMD_INVALID},
					/*<< Get Features */
	{ONFI_CMD_SET_FEATURES, XNANDPSS_END_CMD_NONE, 1,
		XNANDPSS_END_CMD_INVALID},
					/*<< Set Features */
};



/**************************************************************************/
/**
*
* This function reads the data from flash. It is used for reading the control
* information from flash like ID and Parameter page. The data is read in units 
* of one byte.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
* @param	Buf is the buffer pointer to read the data to.
* @param	Length is the length of data to read.
*
* @return	None
*
* @note		None
*
***************************************************************************/
static void Onfi_ReadData(XNandPss *InstancePtr, u8 *Buf, u32 Length)
{
	u32 Index;

	/* 
	 * 8-bit access for basic read/write operations
	 */
	for(Index = 0; Index < Length; Index++) {
		Buf[Index] = Xil_In8(InstancePtr->DataPhaseAddr);
	}
}

/**************************************************************************/
/**
*
* This function reads the data from flash. It is used for reading the control
* information from flash like ID and Parameter page. The data is read in units 
* of one byte.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
* @param	Buf is the buffer pointer to read the data to.
* @param	Length is the length of data to read.
*
* @return	None
*
* @note		None
*
***************************************************************************/
static void Onfi_WriteData(XNandPss *InstancePtr, u8 *Buf, u32 Length)
{
	u32 Index;

	/* 
	 * 8-bit access for basic read/write operations
	 */
	for(Index = 0; Index < Length; Index++) {
		Xil_Out8(InstancePtr->DataPhaseAddr, Buf[Index]);
	}
}

/**************************************************************************/
/**
*
* This function sends read status command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
*
* @return	flash status value read
*
* @note		None
*
***************************************************************************/
u8 Onfi_CmdReadStatus(XNandPss *InstancePtr)
{
	u8 Status;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ_STATUS],
			XNANDPSS_PAGE_NOT_VALID, XNANDPSS_COLUMN_NOT_VALID);

	if(InstancePtr->Geometry.FlashWidth == XNANDPSS_FLASH_WIDTH_16)
		Status = (u8)Xil_In16(InstancePtr->DataPhaseAddr);
	else
		Status = Xil_In8(InstancePtr->DataPhaseAddr);

	return Status;
}

/**************************************************************************/
/**
*
* This function sends reset command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
*
* @return
*   ---- XST_SUCCESS if everything goes well
*   ---- XST_FAILURE if timed out waiting for device to be ready
*
* @note		None
*
***************************************************************************/
static int Onfi_CmdReset(XNandPss *InstancePtr)
{
	u8 Status;
	int Count = ONFI_RESET_LOOP_COUNT;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[RESET],
			XNANDPSS_PAGE_NOT_VALID, XNANDPSS_COLUMN_NOT_VALID);
	
	/* 
	 * Check the Status Register SR[6]
	 */
	do {
		Status = Onfi_CmdReadStatus(InstancePtr);
		Count -= 1;

		if (!Count) {
			return XST_FAILURE;
		}

	}while ((Status & ONFI_STATUS_RDY) != ONFI_STATUS_RDY);

	return XST_SUCCESS;
}

/**************************************************************************/
/**
*
* This function sends read ID command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
*
* @return	None
*
* @note		None
*
***************************************************************************/
static void Onfi_CmdReadId(XNandPss *InstancePtr, u8 Address)
{
	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ_ID],
			XNANDPSS_PAGE_NOT_VALID, Address);
}

/**************************************************************************/
/**
*
* This function sends read parameter page command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
*
* @return
*   ---- XST_SUCCESS if everything goes well
*   ---- XST_FAILURE if timed out waiting for device to be ready
*
* @note		None
*
***************************************************************************/
static int Onfi_CmdReadParamPage(XNandPss *InstancePtr)
{
	u8 Status;
	u32 ZeroCommand;
	int Count = ONFI_RESET_LOOP_COUNT;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ_PARAM_PAGE],
			XNANDPSS_PAGE_NOT_VALID, 0x00);
	/* 
	 * Check the Status Register SR[6]
	 */
	do {
		Status = Onfi_CmdReadStatus(InstancePtr);
		Count -= 1;

		if (!Count) {
			return XST_FAILURE;
		}

	}while ((Status & ONFI_STATUS_RDY) != ONFI_STATUS_RDY);
	
	/* 
	 * ONFI : Reissue the 0x00 on the command line to start reading data
	 */
	ZeroCommand = InstancePtr->Config.FlashBase |
			(0 << XNANDPSS_ADDR_CYCLES_SHIFT)|
			(0 << XNANDPSS_END_CMD_VALID_SHIFT)|
			(XNANDPSS_COMMAND_PHASE_MASK)|
			(0 << XNANDPSS_END_CMD_SHIFT)|
			(0 << XNANDPSS_START_CMD_SHIFT);
	
	/* 
	 * Dummy AXI transaction for sending command 0x00 to the flash
	 */
	Xil_Out32(ZeroCommand, 0x0);

	return XST_SUCCESS;
}

/**************************************************************************/
/**
*
* This function sends Get Feature command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
* @param	Feature is the feature value to read.
*
* @return
*   ---- XST_SUCCESS if everything goes well
*   ---- XST_FAILURE if timed out waiting for device to be ready
*
* @note		None
*
***************************************************************************/
static int Onfi_GetFeature(XNandPss *InstancePtr, u8 Feature, u8 *Val)
{
	u8 Status;
	u32 ZeroCommand;
	int Count = ONFI_RESET_LOOP_COUNT;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[GET_FEATURES],
			XNANDPSS_PAGE_NOT_VALID, Feature);
	/* 
	 * Check the Status Register SR[6]
	 */
	do {
		Status = Onfi_CmdReadStatus(InstancePtr);
		Count -= 1;

		if (!Count) {
			return XST_FAILURE;
		}

	}while ((Status & ONFI_STATUS_RDY) != ONFI_STATUS_RDY);
	
	/* 
	 * ONFI 2.3: Reissue the 0x00 on the command line to start reading
	 * data.
	 */
	ZeroCommand = InstancePtr->Config.FlashBase |
			(0 << XNANDPSS_ADDR_CYCLES_SHIFT)|
			(0 << XNANDPSS_END_CMD_VALID_SHIFT)|
			(XNANDPSS_COMMAND_PHASE_MASK)|
			(0 << XNANDPSS_END_CMD_SHIFT)|
			(0 << XNANDPSS_START_CMD_SHIFT);
	
	/* 
	 * Dummy AXI transaction for sending command 0x00 to the flash
	 */
	Xil_Out32(ZeroCommand, 0x00);

	/*
	 * Read the feature value
	 */
	Onfi_ReadData(InstancePtr, Val, 4);

	return XST_SUCCESS;
}

/**************************************************************************/
/**
*
* This function sends Get Feature command to the flash device.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
* @param	Feature is the feature value to read.
*
* @return
*   ---- XST_SUCCESS if everything goes well
*   ---- XST_FAILURE if timed out waiting for device to be ready
*
* @note		None
*
***************************************************************************/
static int Onfi_SetFeature(XNandPss *InstancePtr, u8 Feature, u8 *Val)
{
	u8 Status;
	int Count = ONFI_RESET_LOOP_COUNT;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[SET_FEATURES],
			XNANDPSS_PAGE_NOT_VALID, Feature);

	Onfi_WriteData(InstancePtr, Val, 4);

	/* 
	 * Check the Status Register SR[6]
	 */
	do {
		Status = Onfi_CmdReadStatus(InstancePtr);
		Count -= 1;

		if (!Count) {
			return XST_FAILURE;
		}

	}while ((Status & ONFI_STATUS_RDY) != ONFI_STATUS_RDY);

	return XST_SUCCESS;
}

/**************************************************************************/
/**
*
* This function calculates the CRC on the parameter page buffer. This is taken
* from the ONFI 1.0 specification.
*
* @param	Buf is the parameter page buffer.
*
* @return	CRC value calculated.
*
* @note		None
*
***************************************************************************/
static unsigned long Onfi_Crc16(u8 *Buf)
{
	const int Order = ONFI_CRC_ORDER;
	const unsigned long Polynom = ONFI_CRC_POLYNOM;
	u32 Crc = ONFI_CRC_INIT; 
	u32 Index;
	u32 j;
	u32 c;
	u32 Bit;
	u32 DataIn;
	int DataByteCount = 0;
	u32 CrcMask = ((((u32)1 << (Order - 1)) -1) << 1) | 1;
	u32 CrcHighBit = (u32)1 << (Order - 1);
	
	/* 
	 * CRC covers the data bytes between byte 0 and byte 253 (ONFI 1.0, sec
	 * 5.4.1.36)
	 */
	for(Index = 0; Index < ONFI_CRC_LEN; Index++)
	{
		DataIn = Buf[Index];
		c = (u32)DataIn;
		DataByteCount++;
		for(j = 0x80; j; j >>= 1) {
			Bit = Crc & CrcHighBit;
			Crc <<= 1;
			if (c & j) Bit ^= CrcHighBit;
			if (Bit) Crc ^= Polynom;
		}
		Crc &= CrcMask;
	}
	return Crc;
}

/**************************************************************************/
/**
*
* This function reads the NAND flash parameter page defined by ONFI 1.0 
* specfication.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
* @param	Buf is a buffer pointer to fill the data. 
*
* @return	
* 		- XST_SUCCESS if parameter page read successfully.
* 		- XST_FAILURE if parameter page is not read successfully.
*
* @note		None
*
***************************************************************************/
static int Onfi_ReadParamPage(XNandPss *InstancePtr, u8 *Buf)
{
	u32 Index;
	u32 CrcCalc;
	OnfiNand_Geometry *Geometry;
	
	/* 
	 * Read the first 256 bytes of parameter page
	 */
	Onfi_CmdReadParamPage(InstancePtr);
	/* Read the 3 mandatory parameter pages */
	for(Index = 0; Index < 3; Index++) {
		Onfi_ReadData(InstancePtr, Buf, ONFI_PARAM_PAGE_LEN);
		Geometry = (OnfiNand_Geometry *)Buf;
		/* Check the CRC */
		CrcCalc = Onfi_Crc16(Buf);
		if(CrcCalc == Geometry->Crc) {
			break;
		}
	}

	if (Index >= 3) {
		return XST_FAILURE;
	}

	return XST_SUCCESS;
}
/**************************************************************************/
/**
*
* This function initializes the NAND flash and gets the geometry information.
*
* @param	InstancePtr is a pointer to the XNandPss instance. 
*
* @return
*		- XST_SUCCESS if successful.
*		- XST_FAILURE if failed. 
*
* @note		None
*
***************************************************************************/
int Onfi_NandInit(XNandPss *InstancePtr)
{
	u32 Target;
	int Status;
	u8 Id[ONFI_ID_LEN];
	u8 JedecId[2];
	u8 EccSetFeature[4] = {0x08, 0x00, 0x00, 0x00};
	u8 EccGetFeature[4];
	OnfiNand_Geometry Nand_Geometry;

	Xil_AssertNonvoid(InstancePtr != NULL);

	for(Target=0; Target < XNANDPSS_MAX_TARGETS; Target++) {
		/* 
		 * Reset the target
		 */
		Status = Onfi_CmdReset(InstancePtr);
		if (Status != XST_SUCCESS) {
			debug_xil_printf("Onfi reset timed out\n\r");
			return XST_FAILURE;
		}

		/* 
		 * Read the ONFI ID
		 */
		Onfi_CmdReadId(InstancePtr, 0x20);
		Onfi_ReadData(InstancePtr, &Id[0], ONFI_ID_LEN);
		
		/* 
		 * Check the ONFI signature to know that the target supports
		 * ONFI
		 */
		if (Id[0]=='O' && Id[1]=='N' && Id[2]=='F' && Id[3]=='I') {
			/* Read the parameter page structure */
			Status = Onfi_ReadParamPage(InstancePtr,
					(u8 *)&Nand_Geometry);
			if (Status != XST_FAILURE) {
				InstancePtr->Geometry.NumLun =
					Nand_Geometry.NumLuns;
				InstancePtr->Geometry.PagesPerBlock =
					Nand_Geometry.PagesPerBlock;
				InstancePtr->Geometry.SpareBytesPerPage =
					Nand_Geometry.SpareBytesPerPage;
				InstancePtr->Geometry.BytesPerPage =
					Nand_Geometry.BytesPerPage;
				InstancePtr->Geometry.BlocksPerLun =
					Nand_Geometry.BlocksPerLun;
				InstancePtr->Geometry.NumBlocks =
					(Nand_Geometry.NumLuns *
					 InstancePtr->Geometry.BlocksPerLun);
				InstancePtr->Geometry.NumPages =
					(Nand_Geometry.NumLuns *
					 Nand_Geometry.BlocksPerLun *
					 Nand_Geometry.PagesPerBlock);
				InstancePtr->Geometry.BlockSize =
					 (Nand_Geometry.PagesPerBlock *
					 Nand_Geometry.BytesPerPage);
				InstancePtr->Geometry.DeviceSize =
					(InstancePtr->Geometry.NumBlocks *
					 InstancePtr->Geometry.PagesPerBlock *
					 InstancePtr->Geometry.BytesPerPage);

				/*
				 * Calculate the address cycles
				 */
				InstancePtr->Geometry.RowAddrCycles =
					(Nand_Geometry.AddrCycles & 0xf);
				InstancePtr->Geometry.ColAddrCycles =
					((Nand_Geometry.AddrCycles >> 4) & 0xf);

				OnfiCommands[READ].AddrCycles =
						(InstancePtr->Geometry.RowAddrCycles +
						InstancePtr->Geometry.ColAddrCycles);

				OnfiCommands[PAGE_PROGRAM].AddrCycles =
						(InstancePtr->Geometry.RowAddrCycles +
						InstancePtr->Geometry.ColAddrCycles);

				OnfiCommands[BLOCK_ERASE].AddrCycles =
						InstancePtr->Geometry.RowAddrCycles;

				OnfiCommands[CHANGE_READ_COLUMN].AddrCycles =
						InstancePtr->Geometry.ColAddrCycles;

				OnfiCommands[CHANGE_WRITE_COLUMN].AddrCycles =
						InstancePtr->Geometry.ColAddrCycles;

				/*
				 * Read JEDEC ID
				 */
				Onfi_CmdReadId(InstancePtr, 0x00);
				Onfi_ReadData(InstancePtr, &JedecId[0], 2);

				/* Initialize ECC Mode */
				if (((Nand_Geometry.Revision & (1 << 5)) ||
					(Nand_Geometry.Revision & (1 << 6))) &&
					(Nand_Geometry.Features & (1 << 9))) {
					/* 
					 * Check if this flash is compliant with
					 * ONFI 2.3 or 3.0 specification and
					 * supports EZ NAND feature.
				 	 */
					InstancePtr->EccMode = XNANDPSS_ECC_FLASH;
				} else if ((JedecId[0] == 0x2C) &&
						/* 1 Gb flash devices */
						((JedecId[1] == 0xF1) ||
						(JedecId[1] == 0xA1) ||
						(JedecId[1] == 0xB1) ||
						/* 2 Gb flash devices */
						(JedecId[1] == 0xAA) ||
						(JedecId[1] == 0xBA) ||
						(JedecId[1] == 0xDA) ||
						(JedecId[1] == 0xCA) ||
						/* 4 Gb flash devices */
						(JedecId[1] == 0xAC) ||
						(JedecId[1] == 0xBC) ||
						(JedecId[1] == 0xDC) ||
						(JedecId[1] == 0xCC) ||
						/* 8 Gb flash devices */
						(JedecId[1] == 0xA3) ||
						(JedecId[1] == 0xB3) ||
						(JedecId[1] == 0xD3) ||
						(JedecId[1] == 0xC3))) {
					/*
					 * Check if this flash supports On-Die ECC.
					 * Micron Flash: MT29F1G08ABADA, MT29F1G08ABBDA
					 *		 MT29F1G16ABBDA,
					 *		 MT29F2G08ABBEA, MT29F2G16ABBEA,
					 *		 MT29F2G08ABAEA, MT29F2G16ABAEA,
					 *		 MT29F4G08ABBDA, MT29F4G16ABBDA,
					 *		 MT29F4G08ABADA, MT29F4G16ABADA
					 *		 MT29F8G08ADBDA, MT29F8G16ADBDA
					 *		 MT29F8G08ADADA, MT29F8G16ADADA
					 */

					Onfi_SetFeature(InstancePtr, 0x90,
								&EccSetFeature[0]);
					/* Check to see if ECC feature is set */
					Onfi_GetFeature(InstancePtr, 0x90,
								&EccGetFeature[0]);
					if (EccGetFeature[0] & 0x08) {
						InstancePtr->EccMode = XNANDPSS_ECC_ONDIE;
					} else {
						InstancePtr->EccMode = XNANDPSS_ECC_HW;
					}
				} else if (Nand_Geometry.BytesPerPage < 512 ||
					Nand_Geometry.BytesPerPage > 2048) {
					/*
					 * This controller doesn't support ECC for
					 * page size < 512 & > 2048 bytes.
					 */
					InstancePtr->EccMode = XNANDPSS_ECC_NONE;
				} else {
					/* SMC controller ECC (1-bit correction) */
					InstancePtr->EccMode = XNANDPSS_ECC_HW;
				}

				/* Initializing width after Get/Set features commands */
				InstancePtr->Geometry.FlashWidth =
					(Nand_Geometry.Features & 0x1)?  16:8;

			} else {
				return XST_FAILURE;
			}
		} else {
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}
