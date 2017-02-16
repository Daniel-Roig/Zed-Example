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
* @file xnandpss_bbm.c
* This file implements the Bad Block Management (BBM) functionality.
* See xnandpss_bbm.h for more details.
*
* @note         None
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who    Date        Changes
* ----- ----   ----------  -----------------------------------------------
* 1.00a nm     12/10/2010  First release
* </pre>
*
******************************************************************************/

/***************************** Include Files *********************************/
#include "xil_types.h"
#include "xnandpss_bbm.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static int XNandPss_ReadBbt(XNandPss *InstancePtr);

static int XNandPss_SearchBbt(XNandPss *InstancePtr, XNandPss_BbtDesc *Desc);

static void XNandPss_CreateBbt(XNandPss *InstancePtr);

static void XNandPss_ConvertBbt(XNandPss *InstancePtr, u8 *Buf);

static int XNandPss_MarkBbt(XNandPss* InstancePtr, XNandPss_BbtDesc *Desc);

static int XNandPss_UpdateBbt(XNandPss *InstancePtr);

extern void XNandPss_ReadSpareBytes(XNandPss *InstancePtr, u32 Page, u8 *Buf);
/************************** Variable Definitions *****************************/

/*****************************************************************************/
/**
* This function initializes the Bad Block Table(BBT) descriptors with a
* predefined pattern for searching Bad Block Table(BBT) in flash.
*
* @param        InstancePtr is a pointer to the XNandPss instance.
*
* @return
*               - NONE
*
******************************************************************************/
void XNandPss_InitBbtDesc(XNandPss *InstancePtr)
{
	int Index;

	Xil_AssertVoid(InstancePtr != NULL);
	Xil_AssertVoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	/*
	 * Initialize primary Bad Block Table(BBT)
	 */
	InstancePtr->BbtDesc.PageOffset = XNANDPSS_BBT_DESC_PAGE_OFFSET;

	if(InstancePtr->EccMode == XNANDPSS_ECC_ONDIE) {
		InstancePtr->BbtDesc.SigOffset = 0x4;
		InstancePtr->BbtDesc.VerOffset = 0x14;
	} else {
		InstancePtr->BbtDesc.SigOffset = XNANDPSS_BBT_DESC_SIG_OFFSET;
		InstancePtr->BbtDesc.VerOffset = XNANDPSS_BBT_DESC_VER_OFFSET;
	}

	InstancePtr->BbtDesc.SigLength = XNANDPSS_BBT_DESC_SIG_LEN;
	InstancePtr->BbtDesc.MaxBlocks = XNANDPSS_BBT_DESC_MAX_BLOCKS;
	strcpy_rom(&InstancePtr->BbtDesc.Signature[0], "Bbt0");
	InstancePtr->BbtDesc.Version = 0;
	InstancePtr->BbtDesc.Valid = 0;

	/*
	 * Initialize mirror Bad Block Table(BBT)
	 */
	InstancePtr->BbtMirrorDesc.PageOffset = XNANDPSS_BBT_DESC_PAGE_OFFSET;

	if(InstancePtr->EccMode == XNANDPSS_ECC_ONDIE) {
		InstancePtr->BbtMirrorDesc.SigOffset = 0x4;
		InstancePtr->BbtMirrorDesc.VerOffset = 0x14;
	} else {
		InstancePtr->BbtMirrorDesc.SigOffset = XNANDPSS_BBT_DESC_SIG_OFFSET;
		InstancePtr->BbtMirrorDesc.VerOffset = XNANDPSS_BBT_DESC_VER_OFFSET;
	}

	InstancePtr->BbtMirrorDesc.SigLength = XNANDPSS_BBT_DESC_SIG_LEN;
	InstancePtr->BbtMirrorDesc.MaxBlocks = XNANDPSS_BBT_DESC_MAX_BLOCKS;
	strcpy_rom(&InstancePtr->BbtMirrorDesc.Signature[0], "1tbB");
	InstancePtr->BbtMirrorDesc.Version = 0;
	InstancePtr->BbtMirrorDesc.Valid = 0;

	/*
	 * Initialize Bad block search pattern structure
	 */
	if (InstancePtr->Geometry.BytesPerPage > 512) {

		/* For flash page size > 512 bytes */
		InstancePtr->BbPattern.Options = XNANDPSS_BBT_SCAN_2ND_PAGE;
		InstancePtr->BbPattern.Offset =
		               XNANDPSS_BB_PATTERN_OFFSET_LARGE_PAGE;
		InstancePtr->BbPattern.Length =
		               XNANDPSS_BB_PATTERN_LENGTH_LARGE_PAGE;
	} else {
		InstancePtr->BbPattern.Options = XNANDPSS_BBT_SCAN_2ND_PAGE;
		InstancePtr->BbPattern.Offset =
		               XNANDPSS_BB_PATTERN_OFFSET_SMALL_PAGE;
		InstancePtr->BbPattern.Length =
		               XNANDPSS_BB_PATTERN_LENGTH_SMALL_PAGE;
	}

	for(Index=0; Index < XNANDPSS_BB_PATTERN_LENGTH_LARGE_PAGE;
	        Index++) {

		InstancePtr->BbPattern.Pattern[Index] = XNANDPSS_BB_PATTERN;
	}
}

/*****************************************************************************/
/**
* This function scans the NAND flash for factory marked bad blocks and creates
* a RAM based Bad Block Table(BBT).
*
* @param        InstancePtr is a pointer to the XNandPss instance.
*
* @return
*               - NONE
*
******************************************************************************/
static void XNandPss_CreateBbt(XNandPss *InstancePtr)
{
	u32 BlockIndex;
	u32 PageIndex;
	u32 Length;
	u32 BlockOffset;
	u32 BlockShift;
	u32 NumPages;
	u32 Page;
	u8 Buf[XNANDPSS_MAX_SPARE_SIZE];
	u32 BbtLen = InstancePtr->Geometry.NumBlocks >>
	                        XNANDPSS_BBT_BLOCK_SHIFT;

	/*
	 * Number of pages to search for bad block pattern
	 */
	if (InstancePtr->BbPattern.Options & XNANDPSS_BBT_SCAN_2ND_PAGE) {
		NumPages = 2;
	} else {
		NumPages = 1;
	}

	/*
	 * Zero the RAM based Bad Block Table(BBT) entries
	 */
	memset_rom(&InstancePtr->Bbt[0], 0, BbtLen);

	/*
	 * Scan all the blocks for factory marked bad blocks
	 */
	for (BlockIndex = 0; BlockIndex < InstancePtr->Geometry.NumBlocks;
	        BlockIndex++) {
		/*
		 * Block offset in Bad Block Table(BBT) entry
		 */
		BlockOffset = BlockIndex >> XNANDPSS_BBT_BLOCK_SHIFT;

		/*
		 * Block shift value in the byte
		 */
		BlockShift = XNandPss_BbtBlockShift(BlockIndex);
		Page = BlockIndex * InstancePtr->Geometry.PagesPerBlock;

		/*
		 * Search for the bad block pattern
		 */
		for (PageIndex = 0; PageIndex < NumPages; PageIndex++) {

			XNandPss_ReadSpareBytes(InstancePtr,
			                        (Page + PageIndex),
			                        &Buf[0]);

			/*
			 * Read the spare bytes to check for bad block
			 * pattern
			 */
			for (Length = 0; Length < InstancePtr->BbPattern.Length;
			        Length++) {

				if (Buf[InstancePtr->BbPattern.Offset + Length] !=
				      InstancePtr->BbPattern.Pattern[Length]) {

					/* Bad block found */
					InstancePtr->Bbt[BlockOffset] |=
					     (XNANDPSS_BLOCK_FACTORY_BAD << BlockShift);

					break;
				}
			}
		}
	}
}

/*****************************************************************************/
/**
* This function reads the Bad Block Table(BBT) if present in flash. If not it
* scans the flash for detecting factory marked bad blocks and creates a bad
* block table and write the Bad Block Table(BBT) into the flash.
*
* @param        InstancePtr is a pointer to the XNandPss instance.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
int XNandPss_ScanBbt(XNandPss *InstancePtr)
{

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);

	if (XNandPss_ReadBbt(InstancePtr) != XST_SUCCESS) {
		/*
		 * Create memory based Bad Block Table(BBT)
		 */
		XNandPss_CreateBbt(InstancePtr);

		/*
		 * Mark the blocks containing Bad Block Table(BBT) as Reserved
		 */
		XNandPss_MarkBbt(InstancePtr, &InstancePtr->BbtDesc);
		XNandPss_MarkBbt(InstancePtr, &InstancePtr->BbtMirrorDesc);
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function converts the Bad Block Table(BBT) read from the flash to the RAM
* based Bad Block Table(BBT).
*
* @param        InstancePtr is a pointer to the XNandPss instance.
* @param        Buf is the buffer which contains BBT read from flash.
*
* @return
*               - NONE.
*
******************************************************************************/
static void XNandPss_ConvertBbt(XNandPss *InstancePtr, u8 *Buf)
{
	u32 BlockOffset;
	u32 BlockShift;
	u32 Data;
	u8 BlockType;
	u32 BlockIndex;
	u32 BbtLen = InstancePtr->Geometry.NumBlocks >>
	                            XNANDPSS_BBT_BLOCK_SHIFT;

	for(BlockOffset = 0; BlockOffset < BbtLen; BlockOffset++) {

		Data = Buf[BlockOffset];

		/*
		 * Clear the RAM based Bad Block Table(BBT) contents
		 */
		InstancePtr->Bbt[BlockOffset] = 0x0;

		/*
		 * Loop through the every 4 blocks in the bitmap
		 */
		for(BlockIndex = 0; BlockIndex < XNANDPSS_BBT_ENTRY_NUM_BLOCKS;
		         BlockIndex++) {

			BlockShift = XNandPss_BbtBlockShift(BlockIndex);
			BlockType = (Data >> BlockShift) &
			                  XNANDPSS_BLOCK_TYPE_MASK;

			switch(BlockType) {
			case XNANDPSS_FLASH_BLOCK_FACTORY_BAD:
				/* Factory bad block */
				InstancePtr->Bbt[BlockOffset] |=
				        XNANDPSS_BLOCK_FACTORY_BAD << BlockShift;
				break;

			case XNANDPSS_FLASH_BLOCK_RESERVED:
				/* Reserved block */
				InstancePtr->Bbt[BlockOffset] |=
				        XNANDPSS_BLOCK_RESERVED << BlockShift;
				break;

			case XNANDPSS_FLASH_BLOCK_BAD:
				/* Bad block due to wear */
				InstancePtr->Bbt[BlockOffset] |=
				             XNANDPSS_BLOCK_BAD << BlockShift;
				break;

			default:
				/* Good block */
				/* The BBT entry already defaults to
				 * zero */
				break;
			}
		}
	}
}

/*****************************************************************************/
/**
* This function searches the Bad Bloock Table(BBT) in flash and loads into the
* memory based Bad Block Table(BBT).
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_ReadBbt(XNandPss *InstancePtr)
{
	u64 Offset;
	u8 Buf[XNANDPSS_MAX_BLOCKS >> XNANDPSS_BBT_BLOCK_SHIFT];
	u32 Status1;
	u32 Status2;
	u32 BbtLen;

	XNandPss_BbtDesc *Desc = &InstancePtr->BbtDesc;
	XNandPss_BbtDesc *MirrorDesc = &InstancePtr->BbtMirrorDesc;
	BbtLen = InstancePtr->Geometry.NumBlocks >> XNANDPSS_BBT_BLOCK_SHIFT;

	/*
	 * Search the Bad Block Table(BBT) in flash
	 */
	Status1 = XNandPss_SearchBbt(InstancePtr, Desc);
	Status2 = XNandPss_SearchBbt(InstancePtr, MirrorDesc);
	if ((Status1 != XST_SUCCESS) && (Status2 != XST_SUCCESS)) {
		return XST_FAILURE;
	}

	/*
	 * Bad Block Table found
	 */
	if (Desc->Valid && MirrorDesc->Valid) {

		/*
		 * Valid BBT & Mirror BBT found
		 */
		if (Desc->Version >= MirrorDesc->Version) {

			Offset = (u64)Desc->PageOffset *
			           InstancePtr->Geometry.BytesPerPage;

			XNandPss_Read(InstancePtr, Offset, BbtLen, &Buf);

			/*
			 * Convert flash BBT to memory based BBT
			 */
			XNandPss_ConvertBbt(InstancePtr, &Buf[0]);

			MirrorDesc->Version = Desc->Version;

		} else {

			Offset = (u64)MirrorDesc->PageOffset *
			          InstancePtr->Geometry.BytesPerPage;
			XNandPss_Read(InstancePtr, Offset, BbtLen, &Buf);

			/*
			 * Convert flash BBT to memory based BBT
			 */
			XNandPss_ConvertBbt(InstancePtr, &Buf[0]);

			Desc->Version = MirrorDesc->Version;
		}

	} else if (Desc->Valid) {
		/*
		 * Valid Primary BBT found
		 */
		Offset = (u64)Desc->PageOffset * InstancePtr->Geometry.BytesPerPage;
		XNandPss_Read(InstancePtr, Offset, BbtLen, &Buf);

		/*
		 * Convert flash BBT to memory based BBT
		 */
		XNandPss_ConvertBbt(InstancePtr, &Buf[0]);
		MirrorDesc->Version = Desc->Version;

	} else {
		/*
		 * Valid Mirror BBT found
		 */
		Offset = (u64)MirrorDesc->PageOffset *
		           InstancePtr->Geometry.BytesPerPage;
		XNandPss_Read(InstancePtr, Offset, BbtLen, &Buf);

		/*
		 * Convert flash BBT to memory based BBT
		 */
		XNandPss_ConvertBbt(InstancePtr, &Buf[0]);
		Desc->Version = MirrorDesc->Version;

	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function searches the BBT in flash.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @param        Desc is the BBT descriptor pattern to search.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_SearchBbt(XNandPss *InstancePtr, XNandPss_BbtDesc *Desc)
{
	u32 StartBlock;
	u32 SigOffset;
	u32 VerOffset;
	u32 MaxBlocks;
	u32 PageOff;
	u32 SigLength;
	u8 Buf[XNANDPSS_MAX_SPARE_SIZE];
	u32 Block;
	u32 Offset;

	/* Start from the last block */
	StartBlock = InstancePtr->Geometry.NumBlocks - 1;
	SigOffset = Desc->SigOffset;
	VerOffset = Desc->VerOffset;
	MaxBlocks = Desc->MaxBlocks;
	SigLength = Desc->SigLength;

	/*
	 * Read the last 4 blocks for Bad Block Table(BBT) signature
	 */
	for(Block = 0; Block < MaxBlocks; Block++) {
		PageOff = (StartBlock - Block) *
		           InstancePtr->Geometry.PagesPerBlock;

		XNandPss_ReadSpareBytes(InstancePtr, PageOff, &Buf[0]);

		/*
		 * Check the Bad Block Table(BBT) signature
		 */
		for(Offset = 0; Offset < SigLength; Offset++) {

			if (Buf[Offset + SigOffset] != Desc->Signature[Offset])	{
				break; /* Check the next blocks */
			}
		}

		if (Offset >= SigLength) {
			/*
			 * Bad Block Table(BBT) found
			 */
			Desc->PageOffset = PageOff;
			Desc->Version = Buf[VerOffset];
			Desc->Valid = 1;

			return XST_SUCCESS;
		}
	}

	/*
	 * Bad Block Table(BBT) not found
	 */
	return XST_FAILURE;
}

/*****************************************************************************/
/**
* This function writes Bad Block Table(BBT) from RAM to flash.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @param        Desc is the BBT descriptor to be written to flash.
* @param        MirrorDesc is the mirror BBT descriptor.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_WriteBbt(XNandPss *InstancePtr, XNandPss_BbtDesc *Desc,
                                XNandPss_BbtDesc *MirrorDesc)
{
	u32 Block = InstancePtr->Geometry.NumBlocks - 1;
	u8 Buf[XNANDPSS_MAX_BLOCKS >> XNANDPSS_BBT_BLOCK_SHIFT];
	u8 Mask[4] = {0x00, 0x01, 0x02, 0x03};
	u8 Data;
	u32 BlockOffset;
	u32 BlockShift;
	u32 BlockIndex;
	u32 Index;
	u8 BlockType;
	u32 BbtLen = InstancePtr->Geometry.NumBlocks >>
	                            XNANDPSS_BBT_BLOCK_SHIFT;

	/*
	 * Find a valid block to write the Bad Block Table(BBT)
	 */
	if (!Desc->Valid) {
		for (Index = 0; Index < Desc->MaxBlocks; Index++) {

			Block  = Block - Index;
			BlockOffset = Block >> XNANDPSS_BBT_BLOCK_SHIFT;
			BlockShift = XNandPss_BbtBlockShift(Block);
			BlockType = (InstancePtr->Bbt[BlockOffset] >>
			           BlockShift) & XNANDPSS_BLOCK_TYPE_MASK;

			switch (BlockType) {
			case XNANDPSS_BLOCK_BAD:
			case XNANDPSS_BLOCK_FACTORY_BAD:
				continue;

			default:
				/* Found Good Block */
				break;
			}

			Desc->PageOffset = Block *
			                    InstancePtr->Geometry.PagesPerBlock;

			if (Desc->PageOffset != MirrorDesc->PageOffset) {

				/* Free block found, not used by the other desc */
				Desc->Valid = 1;
				break;
			}
		}

		/*
		 * Block not found for writing Bad Block Table(BBT)
		 */
		if (Index >= Desc->MaxBlocks) {
			return XST_FAILURE;
		}

	} else {
		Block = Desc->PageOffset / InstancePtr->Geometry.PagesPerBlock;
	}

	/*
	 * Convert the memory based BBT to flash based table
	 */
	memset_rom(Buf, 0xff, BbtLen);

	/*
	 * Loop through the number of blocks
	 */
	for(BlockOffset = 0; BlockOffset < BbtLen; BlockOffset++) {

		Data = InstancePtr->Bbt[BlockOffset];

		/*
		 * Calculate the bit mask for 4 blocks at a time in loop
		 */
		for(BlockIndex = 0; BlockIndex < XNANDPSS_BBT_ENTRY_NUM_BLOCKS;
		      BlockIndex++) {

			BlockShift = XNandPss_BbtBlockShift(BlockIndex);
			Buf[BlockOffset] &= ~(Mask[Data &
			               XNANDPSS_BLOCK_TYPE_MASK] << BlockShift);

			Data >>= XNANDPSS_BBT_BLOCK_SHIFT;
		}
	}


	/*
	 * Write the signature and version in the spare data area
	 */
	memset_rom(InstancePtr->SpareBufPtr, 0xff,
	               InstancePtr->Geometry.SpareBytesPerPage);

	memcpy_rom(InstancePtr->SpareBufPtr + Desc->SigOffset,
	               &Desc->Signature[0], Desc->SigLength);

	memcpy_rom(InstancePtr->SpareBufPtr + Desc->VerOffset,
	               &Desc->Version, 1);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function updates the primary and mirror Bad Block Table(BBT) in the
* flash.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_UpdateBbt(XNandPss *InstancePtr)
{
	int Status;
	u8 Version;

	/*
	 * Update the version number
	 */
	Version = InstancePtr->BbtDesc.Version;
	InstancePtr->BbtDesc.Version = (Version + 1) % 256;
	Version = InstancePtr->BbtMirrorDesc.Version;
	InstancePtr->BbtMirrorDesc.Version = (Version + 1) % 256;

	/*
	 * Update the primary Bad Block Table(BBT) in flash
	 */
	Status = XNandPss_WriteBbt(InstancePtr, &InstancePtr->BbtDesc,
	                  &InstancePtr->BbtMirrorDesc);
	if (Status != XST_SUCCESS) {

		return Status;
	}

	/*
	 * Update the mirrored Bad Block Table(BBT) in flash
	 */
	Status = XNandPss_WriteBbt(InstancePtr, &InstancePtr->BbtMirrorDesc,
	                  &InstancePtr->BbtDesc);
	if (Status != XST_SUCCESS) {

		return Status;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
* This function marks the block containing Bad Block Table as reserved
* and updates the BBT.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @param        Desc is the BBT descriptor pointer.
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_MarkBbt(XNandPss* InstancePtr, XNandPss_BbtDesc *Desc)
{
	u32 BlockIndex;
	u32 BlockOffset;
	u8 BlockShift;
	u8 OldVal;
	u8 NewVal;
	int Status;
	u32 UpdateBbt;
	u32 Index;

	/*
	 * Mark the last four blocks as Reserved
	 */
	BlockIndex = InstancePtr->Geometry.NumBlocks - Desc->MaxBlocks - 1;

	for(Index = 0; Index < Desc->MaxBlocks; Index++,BlockIndex++) {

		BlockOffset = BlockIndex >> XNANDPSS_BBT_BLOCK_SHIFT;
		BlockShift = XNandPss_BbtBlockShift(BlockIndex);
		OldVal = InstancePtr->Bbt[BlockOffset];
		NewVal = OldVal | (XNANDPSS_BLOCK_RESERVED << BlockShift);
		InstancePtr->Bbt[BlockOffset] = NewVal;

		if (OldVal != NewVal) {
			UpdateBbt = 1;
		}
	}

	/*
	 * Update the BBT to flash
	 */
	if (UpdateBbt) {
		Status = XNandPss_UpdateBbt(InstancePtr);

		if (Status != XST_SUCCESS) {
			return Status;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function checks whether a block is bad or not.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
*
* @param        Block is the block number.
*
* @return
*    1 -- if Block is bad
*    0 -- if Block is good
*
******************************************************************************/
int XNandPss_IsBlockBad(XNandPss *InstancePtr, u32 Block)
{
	u8 Data;
	u8 BlockShift;
	u8 BlockType;
	u32 BlockOffset;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Block < InstancePtr->Geometry.NumBlocks);

	BlockOffset = Block >> XNANDPSS_BBT_BLOCK_SHIFT;
	BlockShift = XNandPss_BbtBlockShift(Block);
	Data = InstancePtr->Bbt[BlockOffset];   /* Block information in BBT */
	BlockType = (Data >> BlockShift) & XNANDPSS_BLOCK_TYPE_MASK;

	if (BlockType != XNANDPSS_BLOCK_GOOD)
		return 1;
	else
		return 0;
}

