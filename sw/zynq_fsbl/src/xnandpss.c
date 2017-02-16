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
* @file xnandpss.c
*
* This file contains the implementation of the interface functions for
* XNandPss driver. Refer to the header file xnandpss.h for more detailed
* information.
*
* This module supports for NAND flash memory devices that conform to the
* "Open NAND Flash Interface" (ONFI) Specification. This modules implements
* basic flash operations like read, write and erase.
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
#include "xnandpss.h"
#include "xnandpss_bbm.h"
#include "xnandpss_onfi.h"
#include "smc.h"
#include "top_level.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
static int XNandPss_EccInit(XNandPss *InstancePtr);
static int XNandPss_ReadPage_HwEcc(XNandPss *InstancePtr, u32 Page, u32 Col,
                                   u8 *DstPtr);
static int XNandPss_ReadPage(XNandPss *InstancePtr, u32 Page, u32 Col,
                                   u8 *DstPtr);
static int XNandPss_EccCalculate(XNandPss *InstancePtr, u8* Buf, u8 *EccData);
static int XNandPss_EccCorrect(XNandPss *InstancePtr, u8 *Buf, u8 *EccCalc,
                               u8 *EccCode);
static void XNandPss_ReadBuf(XNandPss *InstancePtr, u8 *Buf, u32 Length);
static int XNandPss_IsBusy(XNandPss *InstancePtr);
static u32 div64_32(u64 *n, u32 base);

/************************** Variable Definitions *****************************/
/* ECC data position in the spare data area  for different page sizes */
u32 NandOob16[] = {13, 14, 15};    /**< Ecc position for 16 bytes spare area */

u32 NandOob32[] = {26, 27, 28, 29, 30, 31};
                                /**< Ecc position for 32 bytes spare area */

u32 NandOob64[] = {52, 53, 54, 55, 56, 57,
                   58, 59, 60, 61, 62, 63};
                                /**< Ecc position for 64 bytes spare area */

extern XNandPss_CommandFormat OnfiCommands[];   /**< ONFI commands */

/*****************************************************************************/
/**
*
* This function initializes a specific XNandPss device/instance. This function
* must be called prior to using the flash device to read or write any data.
*
* @param        InstancePtr is a pointer to the XNandPss instance.
* @param        ConfigPtr points to the XNandPss device configuration structure.
* @param        SmcBaseAddr is the base address of SMC controller.
* @param        FlashBaseAddr is the base address of NAND flash.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*               - XST_NAND_PART_NOT_SUPPORTED if the flash is not supported.
*
* @note         The user needs to first call the XNandPss_LookupConfig() API
*               which returns the Configuration structure pointer which is
*               passed as a parameter to the XNandPss_CfgInitialize() API.
*
******************************************************************************/
int XNandPss_CfgInitialize(XNandPss *InstancePtr, XNandPss_Config *ConfigPtr,
                           u32 SmcBaseAddr, u32 FlashBaseAddr)
{
	u32 EccConfig = 0;
	u32 PageSize;
	int Status;
	int Count = ONFI_RESET_LOOP_COUNT;

	/*
	 * Assert the input arguments.
	 */
	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(ConfigPtr != NULL);

	/*
	 * Set the values read from the device config and the base address.
	 */
	InstancePtr->Config.DeviceId = ConfigPtr->DeviceId;
	InstancePtr->Config.SmcBase = SmcBaseAddr;
	InstancePtr->Config.FlashBase = FlashBaseAddr;
	InstancePtr->Config.FlashWidth = ConfigPtr->FlashWidth;

	/* Disable interrupts */
	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
			SMC_MEMC_CFG_CLR_REG,
			XNANDPSS_CLR_CONFIG);

	/* nand_cycles settings */
	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
			SMC_SET_CYCLES_REG,
			XNANDPSS_SET_CYCLES);

	/*
	 * Set_opmode register settings
	 */
	if (InstancePtr->Config.FlashWidth == XNANDPSS_FLASH_WIDTH_16) {
		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		                  SMC_SET_OPMODE_REG,
		                  XNANDPSS_SET_OPMODE_MW_16_BITS);

	} else {
		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		                  SMC_SET_OPMODE_REG,
		                  XNANDPSS_SET_OPMODE_MW_8_BITS);
	}

	/*
	 * Update the set_cycles and opmode values using direct command
	 * register
	 */
	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
			SMC_DIRECT_CMD_REG,
			XNANDPSS_DIRECT_CMD);

	/*
	 * ONFI query to get geometry
	 */
	Status = Onfi_NandInit(InstancePtr);
	if (Status != XST_SUCCESS) {
		debug_xil_printf("Onfi_NandInit failed\n\r");
		return Status;
	}

	/*
	 * Set_opmode register settings
	 */
	if (InstancePtr->Geometry.FlashWidth == XNANDPSS_FLASH_WIDTH_16) {
		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		                  SMC_SET_OPMODE_REG,
		                  XNANDPSS_SET_OPMODE_MW_16_BITS);

	} else {
		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		                  SMC_SET_OPMODE_REG,
		                  XNANDPSS_SET_OPMODE_MW_8_BITS);
	}

	/*
	 * Update the opmode values using direct command
	 * register
	 */
	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
	                  SMC_DIRECT_CMD_REG,
	                  XNANDPSS_DIRECT_CMD);

	/*
	 * Fill the spare buffer pointer
	 */
	PageSize = InstancePtr->Geometry.BytesPerPage;
	InstancePtr->SpareBufPtr = &InstancePtr->DataBuf[PageSize];

	/*
	 * Initialize ECC Block Parameters
	 */
	switch (InstancePtr->EccMode) {
	case XNANDPSS_ECC_NONE:
		/* Fall through */
	case XNANDPSS_ECC_ONDIE:
		/* Fall through */
	case XNANDPSS_ECC_FLASH:
		debug_xil_printf("No ECC (or) On-Die ECC (or) EZ NAND flash\n\r");
		/*
		 * Check the busy status of the ECC block
		 */
		while (XNandPss_ReadReg(InstancePtr->Config.SmcBase +
		       XNANDPSS_ECC_STATUS_OFFSET(XNANDPSS_IF1_ECC_OFFSET)) &
		       XNANDPSS_ECC_STATUS_MASK) {

			Count --;

			if (!Count) {
				debug_xil_printf("ECC busy?\n\r");
				return XST_FAILURE;
			}

		}

		/* Bypass the ECC block in the SMC controller */
		EccConfig = XNandPss_ReadReg(InstancePtr->Config.SmcBase +
		          (XNANDPSS_ECC_MEMCFG_OFFSET(XNANDPSS_IF1_ECC_OFFSET)));

		EccConfig &= ~XNANDPSS_ECC_MEMCFG_ECC_MODE_MASK;

		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		          (XNANDPSS_ECC_MEMCFG_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
		          EccConfig);

		/* Initialize the Read page routines */
		InstancePtr->ReadPage = XNandPss_ReadPage;
		break;

	case XNANDPSS_ECC_HW:
		debug_xil_printf("ECC hw\n\r");

		/* Use SMC ECC controller ECC block */
		Status = XNandPss_EccInit(InstancePtr);
		if (Status != XST_SUCCESS)
			return Status;

		/* Initialize the Read page routines */
		InstancePtr->ReadPage = XNandPss_ReadPage_HwEcc;
		break;

	default:
		debug_xil_printf("Unknown ECC mode %x\n\r",
		                  InstancePtr->EccMode);
		return XST_FAILURE;
	}

	/*
	 * Indicate the instance is now ready to use, initialized without error.
	 */
	InstancePtr->IsReady = XIL_COMPONENT_IS_READY;

	/*
	 * Scan for the bad block table(bbt) stored in the flash & load it in
	 * memory(RAM).  If bbt is not found, create bbt by scanning factory
	 * marked bad blocks and store it in last good blocks of flash.
	 */
	XNandPss_InitBbtDesc(InstancePtr);

	Status = XNandPss_ScanBbt(InstancePtr);
	if (Status != XST_SUCCESS) {
		debug_xil_printf("Scan BBT failed\n\r");
		return Status;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/*
*
* Initialize the ECC controller in SMC, this is used when ECC is enabled and
* the flash or on-ide does not have ECC controller.
*
* @param InstancePtr is the pointer to the hardware instance
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
******************************************************************************/
static int XNandPss_EccInit(XNandPss *InstancePtr)
{
	u32 PageSize;
	u32 SpareBytesSize;
	u32 EccConfig = 0;
	u32 Index;
	int Count = ONFI_RESET_LOOP_COUNT;

	PageSize = InstancePtr->Geometry.BytesPerPage;
	SpareBytesSize = InstancePtr->Geometry.SpareBytesPerPage;

	/*
	 * Check the busy status of the ECC block
	 */
	while (XNandPss_ReadReg(InstancePtr->Config.SmcBase +
	    XNANDPSS_ECC_STATUS_OFFSET(XNANDPSS_IF1_ECC_OFFSET)) &
	    XNANDPSS_ECC_STATUS_MASK) {

		Count --;

		if (!Count) {
			debug_xil_printf("ECC busy?\n\r");
			return XST_FAILURE;
		}

	}

	/*
	 * Set the ECC mem command1 and ECC mem command2 register
	 */
	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
	         (XNANDPSS_ECC_MEMCMD1_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
	         XNANDPSS_ECC_CMD1);

	XNandPss_WriteReg(InstancePtr->Config.SmcBase +
	         (XNANDPSS_ECC_MEMCMD2_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
	         XNANDPSS_ECC_CMD2);
	/*
	 * Configure HW ECC block
	 */
	switch(PageSize) {
	case XNANDPSS_PAGE_SIZE_512:
		EccConfig = (XNANDPSS_ECC_MEMCFG |
		             XNANDPSS_ECC_MEMCFG_PAGE_SIZE_512);

		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		       (XNANDPSS_ECC_MEMCFG_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
		       EccConfig);
		break;

	case XNANDPSS_PAGE_SIZE_1024:
		EccConfig = (XNANDPSS_ECC_MEMCFG |
		             XNANDPSS_ECC_MEMCFG_PAGE_SIZE_1024);

		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		       (XNANDPSS_ECC_MEMCFG_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
		       EccConfig);
		break;

	case XNANDPSS_PAGE_SIZE_2048:
		EccConfig = (XNANDPSS_ECC_MEMCFG |
		             XNANDPSS_ECC_MEMCFG_PAGE_SIZE_2048);

		XNandPss_WriteReg(InstancePtr->Config.SmcBase +
		       (XNANDPSS_ECC_MEMCFG_OFFSET(XNANDPSS_IF1_ECC_OFFSET)),
		       EccConfig);
		break;

	default:
		/*
		 * Page size 256 bytes & 4096 bytes not supported
		 * by ECC block
		 */
		debug_xil_printf("Unsupported page size %x\n\r", PageSize);
		return XST_NAND_PART_NOT_SUPPORTED;
	}

	/*
	 * Initialize ECC config structure parameters
	 */
	InstancePtr->EccConfig.BytesPerBlock = XNANDPSS_ECC_BYTES;
	InstancePtr->EccConfig.BlockSize = XNANDPSS_ECC_BLOCK_SIZE;
	InstancePtr->EccConfig.TotalBytes = (PageSize/XNANDPSS_ECC_BLOCK_SIZE)
						* XNANDPSS_ECC_BYTES;
	InstancePtr->EccConfig.NumSteps = PageSize/XNANDPSS_ECC_BLOCK_SIZE;

	/*
	 * Ecc write position in spare data area as per Linux mtd subsystem
	 */
	switch(SpareBytesSize) {
	case XNANDPSS_SPARE_SIZE_16:
		for(Index = 0; Index < InstancePtr->EccConfig.TotalBytes;
		    Index++) {

			InstancePtr->EccConfig.EccPos[Index] = NandOob16[Index];
		}
		break;

	case XNANDPSS_SPARE_SIZE_32:
		for(Index = 0; Index < InstancePtr->EccConfig.TotalBytes;
		    Index++) {

			InstancePtr->EccConfig.EccPos[Index] = NandOob32[Index];
		}
		break;

	case XNANDPSS_SPARE_SIZE_64:
		for(Index = 0; Index < InstancePtr->EccConfig.TotalBytes;
		    Index++) {

			InstancePtr->EccConfig.EccPos[Index] = NandOob64[Index];
		}
		break;

	default:
		debug_xil_printf("Unsupported spare byte size %x\n\r",
		     SpareBytesSize);
		return XST_NAND_PART_NOT_SUPPORTED;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads the data from the Flash device and copies it into the
* specified user buffer. It doesn't check for the bad blocks while reading
* the flash pages that cross block boundary. User must take care of handling
* bad blocks.
*
* @param        InstancePtr is the pointer to the XNandPss instance.
* @param        Offset is the flash data address to read from.
* @param        Length is number of bytes to read.
* @param        DestPtr is the destination address to copy data to.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
* @note         This function reads sequential pages from the Flash device.
*
******************************************************************************/
int XNandPss_Read(XNandPss *InstancePtr, u64 Offset, u32 Length, void *DestPtr)
{
	u32 Page;
	u32 Col;
	u32 PartialBytes;
	u32 NumOfBytes;
	int Status;
	u32 PartialPageRead = 0;
	u32 CopyOffset;
	u8 *BufPtr;
	u8 *Ptr = (u8 *)DestPtr;
	u64 Rem = Offset;

	Xil_AssertNonvoid(InstancePtr != NULL);
	Xil_AssertNonvoid(DestPtr != NULL);
	Xil_AssertNonvoid((Offset + Length) < InstancePtr->Geometry.DeviceSize);
	Xil_AssertNonvoid(InstancePtr->IsReady == XIL_COMPONENT_IS_READY);
	Xil_AssertNonvoid(Length != 0);

	/* Col is the offset into the page
	 * Page is the page number
	 */
	/*
	 * VFP is not enabled during initialization. Using soft division
	 */
	div64_32(&Rem , InstancePtr->Geometry.BytesPerPage);
	Page = (u32)Rem;

	Col = (u32) (Offset & (InstancePtr->Geometry.BytesPerPage - 1));
	PartialBytes = InstancePtr->Geometry.BytesPerPage - Col;
	NumOfBytes = (PartialBytes < Length) ? PartialBytes:Length;
	CopyOffset = InstancePtr->Geometry.BytesPerPage - PartialBytes;

	while (Length) {
		/*
		 * Check if partial read
		 */
		if (NumOfBytes < InstancePtr->Geometry.BytesPerPage) {
			BufPtr = &InstancePtr->DataBuf[0];
			PartialPageRead = 1;
		} else {
			BufPtr = (u8 *)Ptr;
			PartialPageRead = 0;
		}

		/*
		 *  Read the page data
		 */
		Status = InstancePtr->ReadPage(InstancePtr, Page, 0, BufPtr);
		if (Status != XST_SUCCESS) {
			debug_xil_printf("Read page failed\n\r");
			return Status;
		}

		/*
		 * Fill the partial data in the buffer
		 */
		if (PartialPageRead) {
			memcpy_rom(Ptr, BufPtr + CopyOffset, NumOfBytes);
		}

		Ptr += NumOfBytes;
		Length -= NumOfBytes;
		Page++;

		NumOfBytes = (Length > InstancePtr->Geometry.BytesPerPage) ?
			InstancePtr->Geometry.BytesPerPage:Length;

		CopyOffset = 0;
	}

	return XST_SUCCESS;
}


/**************************************************************************/
/**
*
* This function sends a NAND command to the flash device.
*
* @param        InstancePtr is the pointer to XNandPss struture
* @param        Command is the NAND command to send
* @param        Page is the page offset required for specific commands
* @param        Column the column offset required for specific commands
*
* @return       None
*
* @note         None
*
***************************************************************************/
void XNandPss_SendCommand(XNandPss *InstancePtr, XNandPss_CommandFormat
                *Command, int Page, int Column)
{
	u32 EndCmdReq = 0;
	u32 EccLast = 0;
	u32 ClearCs = 0;
	u32 CmdPhaseAddr;
	u32 DataPhaseAddr;
	u32 CmdPhaseData=0;
	u32 PageShift;

	Xil_AssertVoid(Command != NULL);

	if (Command->EndCmdValid == XNANDPSS_CMD_PHASE) {
		EndCmdReq = 1;
	}

	/*
	 * Construct command phase address
	 */
	CmdPhaseAddr = InstancePtr->Config.FlashBase                    |
		(Command->AddrCycles << XNANDPSS_ADDR_CYCLES_SHIFT)     |
		(EndCmdReq << XNANDPSS_END_CMD_VALID_SHIFT)             |
		XNANDPSS_COMMAND_PHASE_MASK                             |
		(Command->EndCmd << XNANDPSS_END_CMD_SHIFT)             |
		(Command->StartCmd << XNANDPSS_START_CMD_SHIFT);

	InstancePtr->CommandPhaseAddr = CmdPhaseAddr;

	EndCmdReq = 0;

	/*
	 * Some NAND commands require end command to be sent after data phase
	 */
	if (Command->EndCmdValid == XNANDPSS_DATA_PHASE) {
		EndCmdReq = 1;
	}

	/*
	 * Construct data phase address
	 */
	DataPhaseAddr = InstancePtr->Config.FlashBase                   |
			(ClearCs << XNANDPSS_CLEAR_CS_SHIFT)            |
			(EndCmdReq << XNANDPSS_END_CMD_VALID_SHIFT)     |
			XNANDPSS_DATA_PHASE_MASK                        |
			(Command->EndCmd << XNANDPSS_END_CMD_SHIFT)     |
			(EccLast << XNANDPSS_ECC_LAST_SHIFT);

	InstancePtr->DataPhaseAddr = DataPhaseAddr;

	/*
	 * Command phase data
	 */
	if ((Column != XNANDPSS_COLUMN_NOT_VALID) && 
		(Page != XNANDPSS_PAGE_NOT_VALID)) {

		if (InstancePtr->Geometry.FlashWidth ==
		                    XNANDPSS_FLASH_WIDTH_16) {

			Column >>= 1;
		}

		/* Send lower bytes of page
		 * address in first address cycle
		 */
		CmdPhaseData = Column;
		PageShift = InstancePtr->Geometry.ColAddrCycles * 8;
		CmdPhaseData |= Page << PageShift;

		XNandPss_WriteReg(CmdPhaseAddr, CmdPhaseData);

		/*
		 * Send the upper bytes of the page address in second address
		 * address cycle
		 */
		CmdPhaseData = Page >> (32 - PageShift) ;

		} else if (Page != XNANDPSS_PAGE_NOT_VALID) {
			CmdPhaseData = Page;

		} else {
			if (InstancePtr->Geometry.FlashWidth ==
			                   XNANDPSS_FLASH_WIDTH_16) {

				Column >>= 1;
		}

		CmdPhaseData = Column;
	}

	/*
	 * Send command phase
	 */
	XNandPss_WriteReg(CmdPhaseAddr, CmdPhaseData);
}

/*****************************************************************************/
/**
*
* This function reads the spare area of a page.
*
* @param        InstancePtr is the pointer to the XNand instance.
* @param        Page is the page number from where spare data is read.
* @param        Buf is pointer to the buffer where the spare data is filled.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
* @note         None.
*
******************************************************************************/
int XNandPss_ReadSpareBytes(XNandPss *InstancePtr, u32 Page, u8 *Buf)
{
	u32 Col;
	u32 Length;
	u32 DataPhaseAddr;
	u32 ZeroCommand;
	u32 Status;
	int Count = ONFI_RESET_LOOP_COUNT;

	Xil_AssertNonvoid(Buf != NULL);

	Col = InstancePtr->Geometry.BytesPerPage;
	Length = InstancePtr->Geometry.SpareBytesPerPage;

	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ], Page, Col);
	/*
	 * Poll the Memory controller status register for BUSY input signal
	 */
	while (XNandPss_IsBusy(InstancePtr) == TRUE) {
		Count --;

		if (!Count) {
			debug_xil_printf("ECC busy?\n\r");
			return XST_FAILURE;
		}
	}

	/*
	 * Clear the interrupt condition
	 */
	XNandPss_WriteReg((InstancePtr->Config.SmcBase +
				SMC_MEMC_CFG_CLR_REG),
				XNANDPSS_MEMC_CLR_CONFIG_INT_CLR1_MASK);

	/*
	 * Check ONFI Status Register
	 */
	Status = Onfi_CmdReadStatus(InstancePtr);
	if (Status & ONFI_STATUS_FAIL) {
		return XST_FAILURE;
	}

	/*
	 * ONFI : Reissue the 0x00 on the command line to start
	 * reading data
	 */
	ZeroCommand = InstancePtr->Config.FlashBase		|
			(0 << XNANDPSS_ADDR_CYCLES_SHIFT)	|
			(0 << XNANDPSS_END_CMD_VALID_SHIFT)	|
			(XNANDPSS_COMMAND_PHASE_MASK)		|
			(0 << XNANDPSS_END_CMD_SHIFT)		|
			(0 << XNANDPSS_START_CMD_SHIFT);

	/*
	 * AXI transaction for sending command 0x00 to the flash
	 */
	Xil_Out32(ZeroCommand, 0x00);

	/*
	 * Read the spare data
	 */
	XNandPss_ReadBuf(InstancePtr, Buf,
	         (Length - XNANDPSS_AXI_DATA_WIDTH));

	/*
	 * Clear chip select for last AXI transaction
	 */
	DataPhaseAddr = InstancePtr->DataPhaseAddr;
	DataPhaseAddr |= XNANDPSS_CLR_CS;
	InstancePtr->DataPhaseAddr = DataPhaseAddr;
	Buf += (Length - XNANDPSS_AXI_DATA_WIDTH);

	XNandPss_ReadBuf(InstancePtr, Buf, XNANDPSS_AXI_DATA_WIDTH);
	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function checks whether SMC controller busy in processing a request.
*
* @param        InstancePtr is a pointer to the XNand instance.
*
* @return       - TRUE if SMC is busy
*               - FALSE if SMC is free
*
* @note         None.
*
******************************************************************************/
static int XNandPss_IsBusy(XNandPss *InstancePtr)
{
        u32 Status;

        /*
         * Read the memory controller status register
         */
        Status = XNandPss_ReadReg(InstancePtr->Config.SmcBase +
                        SMC_STATUS_REG) &
                XNANDPSS_MEMC_STATUS_RAW_INT_STATUS1_MASK;

        if (Status) {
                return FALSE;
        } else {
                return TRUE;
        }
}

/*****************************************************************************/
/**
*
* This function calculates the ECC value from the ECC registers.
*
* @param        InstancePtr is a pointer to the XNand instance.
* @param        Buf is the pointer to buffer to calculate the ECC.
* @param        EccData is the buffer to fill the ECC value.
*
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
* @note         None
*
******************************************************************************/
static int XNandPss_EccCalculate(XNandPss *InstancePtr, u8* Buf, u8 *EccData)
{
	u32 EccReg;
	u32 EccValue;
	u32 EccByte;
	u32 EccStatus;
	int Count = ONFI_RESET_LOOP_COUNT;

	/*
	 * Check the busy status of the ECC block
	 */
	while (XNandPss_ReadReg(InstancePtr->Config.SmcBase +
	        XNANDPSS_ECC_STATUS_OFFSET(XNANDPSS_IF1_ECC_OFFSET)) &
	        XNANDPSS_ECC_STATUS_MASK) {

		Count --;

		if (!Count) {
			debug_xil_printf("ECC busy?\n\r");
			return XST_FAILURE;
		}
	}

	for(EccReg = 0; EccReg < 4; EccReg++) {

		EccValue = XNandPss_ReadReg(InstancePtr->Config.SmcBase +
			XNANDPSS_ECC_VALUE0_OFFSET(XNANDPSS_IF1_ECC_OFFSET +
			(EccReg * 4)));

		EccStatus = (EccValue >> 24) & 0xFF;

		/*
		 * Check if the ECC value not valid
		 */
		if ((EccStatus >> 6) & 0x1) {
			for(EccByte = 0; EccByte < 3; EccByte++) {
				*EccData = EccValue & 0xFF;
				EccValue = EccValue >> 8;
				EccData++;
			}
		} else {
			return XST_FAILURE;
		}
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function corrects the ECC errors.
*
* @param        InstancePtr is a pointer to the XNand instance.
* @param        Buf is the buffer which holds the data read from the page.
* @param        EccCalc is the calculated ECC value.
* @param        EccCode is the ECC read from the spare area.
*
* @return
*               - XST_SUCCESS if the ECC error is corrected.
*               - XST_FAILURE if the ECC error is not corrected.
*
* @note         None
*
******************************************************************************/
static int XNandPss_EccCorrect(XNandPss *InstancePtr, u8 *Buf, u8 *EccCalc,
                 u8 *EccCode)
{
	u8 BitPos;
	u32 BytePos;
	u16 EccOdd, EccEven;
	u16 ReadEccLow, ReadEccHigh;
	u16 CalcEccLow, CalcEccHigh;

	/*
	 * Lower 12 bits of ECC Read
	 */
	ReadEccLow = (EccCode[0] | (EccCode[1] << 8)) & 0xfff;

	/*
	 * Upper 12 bits of ECC Read
	 */
	ReadEccHigh = ((EccCode[1] >> 4) | (EccCode[2] << 4)) & 0xfff;

	/*
	 * Lower 12 bits of ECC calculated
	 */
	CalcEccLow = (EccCalc[0] | (EccCalc[1] << 8)) & 0xfff;

	/*
	 * Upper 12 bits of ECC Calculated
	 */
	CalcEccHigh = ((EccCalc[1] >> 4) | (EccCalc[2] << 4)) & 0xfff;

	EccOdd = ReadEccLow ^ CalcEccLow;
	EccEven = ReadEccHigh ^ CalcEccHigh;

	/*
	 * No Error
	 */
	if ((EccOdd == 0) && (EccEven == 0)) {
		return XST_SUCCESS;
	}

	/*
	 * Single bit error, correct it
	 */
	if (EccOdd == (~EccEven & 0xfff)) {
		BytePos = (EccOdd >> 3) & XNANDPSS_ECC_CORRECT_BYTE_MASK;
		BitPos = EccOdd & XNANDPSS_ECC_CORRECT_BIT_MASK;

		/*
		 * Toggling error bit
		 */
		Buf[BytePos] ^= (1 << BitPos);
		return XST_SUCCESS;
	}

	/*
	 * Parity error
	 */
	if (OneHot((EccOdd | EccEven)) == XST_SUCCESS) {
		return XST_SUCCESS;
	}

	/*
	 * Multiple bit errors
	 */
	return XST_FAILURE;
}

/*****************************************************************************/
/**
*
* This function reads a specific page from NAND device. It checks for the ECC
* errors and corrects single bit errors and reports multiple bit errors to the
* caller.
*
* @param        InstancePtr is a pointer to the XNand instance.
* @param        Page is the page number of the device.
* @param        Col is the column offset in the page to read.
* @param        DstPtr is a pointer to the destination buffer.
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
* @note         None
*
******************************************************************************/
static int XNandPss_ReadPage_HwEcc(XNandPss *InstancePtr, u32 Page,
                 u32 Col, u8 *DstPtr)
{
	u32 Status;
	u32 BytesPerPage;
	u32 SpareBytesPerPage;
	u32 EccSteps;
	u32 EccOffset;
	u32 *EccPos;
	u32 DataPhaseAddr;
	u8 *EccCode;
	u8 *EccCalc;
	u32 Index;
	u8 *Ptr = DstPtr;
	u8 *SparePtr = InstancePtr->SpareBufPtr;
	int Count = ONFI_RESET_LOOP_COUNT;

	BytesPerPage = InstancePtr->Geometry.BytesPerPage;
	SpareBytesPerPage = InstancePtr->Geometry.SpareBytesPerPage;

	EccSteps = InstancePtr->EccConfig.NumSteps;
	EccCode = &InstancePtr->EccCode[0];
	EccCalc = &InstancePtr->EccCalc[0];

	/*
	 * Send the ONFI Read command
	 */
	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ], Page, Col);

	/*
	 * Poll the Memory controller status register
	 */
	while (XNandPss_IsBusy(InstancePtr) == TRUE) {
		Count --;

		if (!Count) {
			debug_xil_printf("ECC busy?\n\r");
			return XST_FAILURE;
		}
	}

	/*
	 * Clear the interrupt condition
	 */
	XNandPss_WriteReg((InstancePtr->Config.SmcBase +
				SMC_MEMC_CFG_CLR_REG),
				XNANDPSS_MEMC_CLR_CONFIG_INT_CLR1_MASK);

	/*
	 * Read page with one data phase transaction left to set ECC
	 */
	XNandPss_ReadBuf(InstancePtr, Ptr,
			(BytesPerPage - XNANDPSS_AXI_DATA_WIDTH));

	Ptr += (BytesPerPage - XNANDPSS_AXI_DATA_WIDTH);

	/*
	 * Set the ECC Last bit
	 */
	DataPhaseAddr = InstancePtr->DataPhaseAddr;
	DataPhaseAddr |= XNANDPSS_ECC_LAST;
	InstancePtr->DataPhaseAddr = DataPhaseAddr;

	/*
	 * Read transaction with ECC enabled
	 */
	XNandPss_ReadBuf(InstancePtr, Ptr, XNANDPSS_AXI_DATA_WIDTH);

	/*
	 * Calculate the hardware ECC
	 */
	Ptr = DstPtr;
	Status = XNandPss_EccCalculate(InstancePtr, Ptr, EccCalc);
	if (Status != XST_SUCCESS) {
		return Status;
	}

	DataPhaseAddr = InstancePtr->DataPhaseAddr;
	DataPhaseAddr &= ~XNANDPSS_ECC_LAST;
	InstancePtr->DataPhaseAddr = DataPhaseAddr;

	XNandPss_ReadBuf(InstancePtr, SparePtr,
			(SpareBytesPerPage - XNANDPSS_AXI_DATA_WIDTH));

	/*
	 * Clear chip select for last AXI transaction
	 */
	DataPhaseAddr = InstancePtr->DataPhaseAddr;
	DataPhaseAddr |= XNANDPSS_CLR_CS;
	InstancePtr->DataPhaseAddr = DataPhaseAddr;
	SparePtr += (SpareBytesPerPage - XNANDPSS_AXI_DATA_WIDTH);
	XNandPss_ReadBuf(InstancePtr, SparePtr, XNANDPSS_AXI_DATA_WIDTH);

	/*
	 * Read the stored ECC code
	 */
	EccPos = &InstancePtr->EccConfig.EccPos[0];
	for(Index = 0; Index < InstancePtr->EccConfig.TotalBytes; Index++) {

		EccCode[Index] = ~(InstancePtr->SpareBufPtr[EccPos[Index]]);
	}

	/*
	 * Check for ECC errors
	 */
	EccOffset = 0;

	for(; EccSteps; EccSteps--) {

		Status = XNandPss_EccCorrect(InstancePtr, DstPtr,
		              &EccCalc[EccOffset],&EccCode[EccOffset]);

		if (Status != XST_SUCCESS) {
			return Status;
		}

		DstPtr += InstancePtr->EccConfig.BlockSize;
		EccOffset += InstancePtr->EccConfig.BytesPerBlock;
	}

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads a specific page from NAND device with out any ECC.
*
* Both data in the page and the data in spare area are read. Page data in
* DstPtr, and spare area data in InstancePtr->SpareBufPtr.
*
* @param        InstancePtr is a pointer to the XNand instance.
* @param        Page is the page number of the device.
* @param        Col is the column offset in the page to read.
* @param        DstPtr is a pointer to the destination buffer.
* @return
*               - XST_SUCCESS if successful.
*               - XST_FAILURE if fail.
*
* @note         None
*
******************************************************************************/
static int XNandPss_ReadPage(XNandPss *InstancePtr, u32 Page, u32 Col,
                u8 *DstPtr)
{
	u32 Status;
	u8 *Ptr = DstPtr;
	u8 *SparePtr = InstancePtr->SpareBufPtr;
	u32 ZeroCommand;
	u32 BytesPerPage;
	u32 SpareBytesPerPage;
	u32 DataPhaseAddr;
	int Count = ONFI_RESET_LOOP_COUNT;

	BytesPerPage = InstancePtr->Geometry.BytesPerPage;
	SpareBytesPerPage = InstancePtr->Geometry.SpareBytesPerPage;

	/*
	 * Send the ONFI Read command
	 */
	XNandPss_SendCommand(InstancePtr, &OnfiCommands[READ], Page, Col);

	/*
	 * Poll the Memory controller status register
	 */
	while (XNandPss_IsBusy(InstancePtr) == TRUE) {
		Count --;

		if (!Count) {
			debug_xil_printf("ECC busy?\n\r");
			return XST_FAILURE;
		}
	}


	/*
	 * Clear the interrupt condition
	 */
	XNandPss_WriteReg((InstancePtr->Config.SmcBase +
				SMC_MEMC_CFG_CLR_REG),
				XNANDPSS_MEMC_CLR_CONFIG_INT_CLR1_MASK);

	/*
	 * Check ONFI Status Register
	 */
	Status = Onfi_CmdReadStatus(InstancePtr);
	if (Status & ONFI_STATUS_FAIL) {
		return XST_FAILURE;
	}

	/*
	 * ONFI : Reissue the 0x00 on the command line to start
	 * reading data
	 */
	ZeroCommand = InstancePtr->Config.FlashBase		|
			(0 << XNANDPSS_ADDR_CYCLES_SHIFT)	|
			(0 << XNANDPSS_END_CMD_VALID_SHIFT)	|
			(XNANDPSS_COMMAND_PHASE_MASK)		|
			(0 << XNANDPSS_END_CMD_SHIFT)		|
			(0 << XNANDPSS_START_CMD_SHIFT);

	/*
	 * AXI transaction for sending command 0x00 to the flash
	 */
	Xil_Out32(ZeroCommand, 0x00);

	/*
	 * Read flash page
	 */
	XNandPss_ReadBuf(InstancePtr, Ptr, BytesPerPage);

	/*
	 * Read flash page in one less AXI data width
	 */
	XNandPss_ReadBuf(InstancePtr, SparePtr,
			(SpareBytesPerPage - XNANDPSS_AXI_DATA_WIDTH));

	/*
	 * Clear chip select for last AXI transaction
	 */
	DataPhaseAddr = InstancePtr->DataPhaseAddr;
	DataPhaseAddr |= XNANDPSS_CLR_CS;
	InstancePtr->DataPhaseAddr = DataPhaseAddr;
	SparePtr += (SpareBytesPerPage - XNANDPSS_AXI_DATA_WIDTH);
	XNandPss_ReadBuf(InstancePtr, SparePtr, XNANDPSS_AXI_DATA_WIDTH);

	return XST_SUCCESS;
}

/*****************************************************************************/
/**
*
* This function reads the page data from the AXI Data Phase Address.
*
* @param        InstancePtr is a pointer to the XNand instance.
* @param        Buf is the buffer pointer to store the byte.
* @param        Length is the number of bytes to read.
*
* @return
*               - None.
*
******************************************************************************/
static void XNandPss_ReadBuf(XNandPss *InstancePtr, u8 *Buf, u32 Length)
{
	u32 Index;
	u32 AxiLen = Length >> 2;
	u32 *Ptr = (u32 *)Buf;

	for(Index = 0; Index < AxiLen; Index++) {
		Ptr[Index] = XNandPss_ReadReg(InstancePtr->DataPhaseAddr);
	}
}

/*****************************************************************************/
/**
* This function used for 64-bit division.
******************************************************************************/
static u32 div64_32(u64 *n, u32 base)
{
	u64 rem = *n;
	u64 b = base;
	u64 res, d = 1;
	u32 high = rem >> 32;

	/* Reduce the thing a bit first */
	res = 0;

	if (high >= base) {
		high /= base;
		res = (u64) high << 32;
		rem -= (u64) (high * base) << 32;
	}

	while ((b > 0) && (b < rem)) {
		b = (u64)b + b;
		d = (u64)d + d;
	}

	do {
		if (rem >= b) {
			rem -= b;
			res += d;
		}

		b >>= 1;
		d >>= 1;
	} while (d);

	*n = res;
	return rem;
}
