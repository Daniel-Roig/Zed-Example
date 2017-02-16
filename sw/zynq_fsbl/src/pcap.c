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
* @file pcap.c
*
* Contains code for enabling and accessing the PCAP
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm  02/10/10 Initial release
* 2.00a jz   05/28/11 Add SD support
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "pcap.h"
#include "mio.h"
#include "nand.h"          /* For NAND geometry information */

#include "top_level.h"
#include "image_mover.h"   /* For MoveImage */

/************************** Constant Definitions *****************************/
/* DEBUG, remove the following defines to remove dump of registers */
#define DUMP_PCAP_REGS

#define MAX_COUNT 1000000
#define WORD_SHIFT 0x2

#define PCAP_LAST_TRANSFER 0x1

#define MAX_BUF_WORD_SIZE       0x1000
#define MAX_BUF_BYTE_SIZE       (MAX_BUF_WORD_SIZE << WORD_SHIFT)
/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;
extern ImageMoverType MoveImage;

extern XNandPss NandInstance;

void pcap_register_dump(void);
u32 NandPageBuffer[MAX_BUF_WORD_SIZE] __attribute__ ((aligned(64)));

/******************************************************************************/
/**
*
* This function reads the PCAP ISR.
*
* @param    none
*
* @return   contents of the PCAP Interrupt Status Register.
*
* @note         none
*
****************************************************************************/
static u32 ReadPcapISR(void){

	u32 RegData;

	RegData = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);

	return RegData;

}

/******************************************************************************/
/**
*
* This function writes the PCAP ISR.
*
* @param    data to be written to PCAP Interrupt Status Register
*
* @return   None
*
* @note         none
*
****************************************************************************/
void WritePcapISR(u32 Data){

	PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR), Data);

	return;

}

/******************************************************************************/
/**
*
* This function reads the PCAP control.
*
* @param    none
*
* @return   contents of the PCAP control register.
*
* @note         none
*
****************************************************************************/
u32 ReadPcapControl(void){

	return PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONTROL_REG);

}

/******************************************************************************/
/**
*
* This function writes the PCAP control.
*
* @param    data to be written to PCAP control
*
* @return   None
*
* @note         none
*
****************************************************************************/
void WritePcapControl(u32 Data){

	PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONTROL_REG), Data);

return;

}

/******************************************************************************/
/**
*
* This function writes the PCAP Lock Register.
*
* @param    data to be written to PCAP Lock Register
*
* @return   None
*
* @note     none
*
****************************************************************************/
void WritePcapLock(u32 Data){

	PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_LOCK_REG), Data);

	return;

}

/******************************************************************************/
/**
*
* This function checks if the PCAP DMA is idle.
*
* @param    none
*
* @return   1 if the DMA is busy
*           0 if the DMA is idle
*
* @note         There could be more than one pending transfer.
*
****************************************************************************/
u32 PcapIsDmaBusy(void){

        u32 RegData;

        /* Read the PCAP status register for DMA status */
        RegData = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG);

        if ((RegData & PCAP_STATUS_DMA_DONE_CNT_MASK) != 0){
                return 1;
        }

        if ((RegData & PCAP_STATUS_DMA_CMD_Q_E_MASK) !=
                                PCAP_STATUS_DMA_CMD_Q_E_MASK){
                return 1;
        }

        return 0;
}

/******************************************************************************/
/**
*
* This function does DMA transfer 
*
* @param SourceData is a pointer to where the data is read from
* @param DestinationData is a pointer to where the data is written to
* @param SourceLength is the length of the data to be moved in words
* @param DestinationLength is the length of the data to be moved in words
* @param Flags indicated the encryption key location, 0 for non-encrypted
*
* @return   XST_SUCCESS if the PCAP transfer is successful
*           XST_FAILURE if the PCAP transfer fails
*           XST_DEVICE_BUSY if the DMA queue is full.
*
* @note         none
*
****************************************************************************/
u32 WritePcapXferData(u32 *SourceData, u32 *DestinationData, u32 SourceLength,
         u32 DestinationLength, u32 Flags)
{
	volatile u32 StatusReg;
	u32 PcapReg;
	u32 Status;
	u32 IntStatusReg;
	int IsBitstream = 0;
	int NonSecure = 0;
	int Count;
	//u32 DefaultPcapControl;

	/*
	 * If serious failures, then:
	 *   . Fallback and Boot ROM will do golden image hunt.
	 *   . If it is an HMAC error, system can only recover if there is a
	 *     non-encrypted golden image
	 */
	IntStatusReg = ReadPcapISR();

	/* Clear it all, so if Boot ROM comes back, it can proceed */
	PcapOut32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR, 0xFFFFFFFF);

	if (IntStatusReg & PCAP_IXR_FATAL_ERROR_MASK) {
		debug_xil_printf("FATAL errors in PCAP %x\r\n", IntStatusReg);

		/* If RX FIFO overflow, need to flush RX FIFO first */
		if (IntStatusReg & PCAP_IXR_RX_FIFO_OV_MASK) {
			PcapOut32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG,
			    PCAP_MCTRL_RFIFO_FLUSH_MASK);

			PcapOut32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR, 0xFFFFFFFF);
		}

		return XST_FAILURE;
	}

	/* Read the PCAP status register for DMA status */
	StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG);

	debug_xil_printf("PCAP:StatusReg = 0x%.8x\r\n", StatusReg);

	/* if the queue is full, return w/ XST_DEVICE_BUSY */
	if ((StatusReg & PCAP_STATUS_DMA_CMD_Q_F_MASK) ==
	        PCAP_STATUS_DMA_CMD_Q_F_MASK) {

		debug_xil_printf("PCAP:device busy\r\n");
		return XST_DEVICE_BUSY;
	}

	debug_xil_printf("PCAP:device ready\r\n");

	/* There are unacknowledged DMA commands outstanding */
	if ((StatusReg & PCAP_STATUS_DMA_CMD_Q_E_MASK) !=
	        PCAP_STATUS_DMA_CMD_Q_E_MASK) {

		IntStatusReg = ReadPcapISR();

		if ((IntStatusReg & PCAP_IXR_DMA_DONE_MASK) !=
		          PCAP_IXR_DMA_DONE_MASK){

			/* error state, transfer cannot occur */
			debug_xil_printf("PCAP:IntStatus indicates error\r\n");
			return XST_FAILURE;
		}
		else {
			/* clear out the status */
			WritePcapISR(PCAP_IXR_DMA_DONE_MASK);
		}
	}

	if ((StatusReg & PCAP_STATUS_DMA_DONE_CNT_MASK) != 0) {

		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG),
		      (StatusReg & PCAP_STATUS_DMA_DONE_CNT_MASK));
	}

	/* Bitstream is treated differently:
	 * . if it is non-secure, then not set loopback mode as PS images
	 * . NAND will use 0xFFFFFFFF for dest address for each sub-transfer
	 * . PCAP completion checks FPGA done bit too
	 */
	if ((u32)DestinationData == PCAP_DMA_INVALID_ADDRESS) {

		IsBitstream = 1;

	} 

	/* Set the correct key source */
	if (Flags == IMAGE_ENCRYPTED_EFUSE){

		/* Set the E-FUSE bit */
		WritePcapControl((ReadPcapControl() |
		        PCAP_CONTROL_PCFG_AES_FUSE_MASK));

		WritePcapLock(PCAP_CONTROL_PCFG_AES_FUSE_MASK);

	} else if (Flags == IMAGE_ENCRYPTED_BBRAM){

		/* Set the BBRAM bit */
		WritePcapControl((ReadPcapControl() &
		      ~PCAP_CONTROL_PCFG_AES_FUSE_MASK));

	}
	else {
	
		NonSecure = 1;

		if (!IsBitstream) {
			/* Set the loopback bit */
			PcapReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG);
			PcapReg |= PCAP_MCTRL_DMAQ_LOOPBACK_MASK;
			PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG),PcapReg);
		}

		PcapReg = ReadPcapControl();

		PcapReg &=  ~PCAP_CONTROL_QTR_PCAP_MASK;

		WritePcapControl(PcapReg);
	}

#ifdef DUMP_PCAP_REGS
	pcap_register_dump();
#endif

	/* NAND/SD only supports bitstream, this has not been decided yet */ 
	if (((FlashReadBaseAddress & 0xFF000000) == XPSS_NAND_BASEADDR) ||
	    (FlashReadBaseAddress == XPSS_SD_BASEADDR)) {
		u32 NandLength;
		u32 TempSourceLength;
		u32 TempDestLength;
		u32 TempSourceAddress;
		u32 BlockMask;
		u32 Block = MAX_BUF_BYTE_SIZE;

		/* For decryption of secure images, do the header transfer
		 * first to line up things */

		BlockMask = Block - 1;

		NandLength = 0;
		TempDestLength = sizeof(NandPageBuffer)/4;

		Count = 0;

		while (NandLength < SourceLength){

			PatWDT();
			TempSourceAddress = (u32)SourceData +
		                     (NandLength << WORD_SHIFT);

			/* TempSourceLength is the amount of data (in words)
			 * to put into temporary buffer 
			 */
			TempSourceLength = (Block - 
		          ((u32)TempSourceAddress  & BlockMask)) >>
		          WORD_SHIFT;

			/* at the end of the xfers, the remaining data
			 * might be less than what is available in the NAND Block
			 */
			if ((TempSourceLength + NandLength) > SourceLength){
				TempSourceLength = SourceLength - NandLength;
			}

			/* Here is where the encrypted partition fails on NAND.
			 * We cannot figure out the correct dest length, DT596707
			 * Unless the tool tells us how much the header/footer are
			 */
			if ((TempDestLength + NandLength) > DestinationLength){

				TempDestLength = DestinationLength - NandLength;
			}else{

				TempDestLength = TempSourceLength;
			}
			
			debug_xil_printf("nand/sd access to address %x with "
			    "length %x\r\n", TempSourceAddress,
			    TempSourceLength << WORD_SHIFT);
		
			/* Read data into OCM */
			Status = MoveImage((u32)TempSourceAddress,
		         (u32)NandPageBuffer,
		         TempSourceLength << WORD_SHIFT);
			if (Status != XST_SUCCESS) {
				debug_xil_printf("PCAP NAND or SD Access FAILURE, "
				      "PCAP_ISR = 0x%.8x\r\n", Status);
				return XST_FAILURE;
			}

			/* set up the transfer
			 * The last transfer has source/destination address
			 * padded with 2b'01
			 */
			if ((NandLength + TempSourceLength) == SourceLength) {

				debug_xil_printf("Last transfer, %d, PCAP: Source = %x\n",
				    Count, (u32)NandPageBuffer | PCAP_LAST_TRANSFER);

				PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_SRC_REG),
			         ((u32)NandPageBuffer | PCAP_LAST_TRANSFER));

				if (IsBitstream) {
					debug_xil_printf("Last transfer, PCAP: destination = %x\n",
						PCAP_DMA_INVALID_ADDRESS);
					PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG),
						PCAP_DMA_INVALID_ADDRESS);
				} else {
					debug_xil_printf("Last transfer, PCAP: destination = %x\n",
					    ((u32)DestinationData + (NandLength << WORD_SHIFT)) |
					    PCAP_LAST_TRANSFER);
					PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG),
					    ((u32)DestinationData + (NandLength << WORD_SHIFT)) |
					    PCAP_LAST_TRANSFER);
				}				

			} else {

				PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_SRC_REG),
			         (u32)NandPageBuffer);

				if (IsBitstream) {
					PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG),
						PCAP_DMA_INVALID_ADDRESS);
				} else {
					PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG),
					    (u32)DestinationData + (NandLength << WORD_SHIFT));
				}
			}

			PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_LEN_REG),
		            TempSourceLength);

			if (IsBitstream) {
				PcapOut32((XPSS_DEV_CFG_APB_BASEADDR +
			               PCAP_DMA_DEST_LEN_REG), 0);
			} else {
				PcapOut32((XPSS_DEV_CFG_APB_BASEADDR +
			               PCAP_DMA_DEST_LEN_REG), TempDestLength);
			}

			NandLength += TempSourceLength;

			/* Polling for completion, this could hang the system */
			StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);
			while((StatusReg & PCAP_IXR_DMA_DONE_MASK) == 0) {

				if ((StatusReg & PCAP_IXR_ERROR_FLAGS_MASK) != 0) {
					debug_xil_printf("FAILURE, PCAP_ISR = 0x%.8x\r\n",
								                  StatusReg);
					return XST_FAILURE;
				}

				StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR +
				                  PCAP_ISR);
			}

			/* clear out the DMA DONE status */
			WritePcapISR(StatusReg & PCAP_IXR_DMA_DONE_MASK);
			Count += 1;
		}

		debug_xil_printf("DMA transfer done %x\n", StatusReg);

		/* If it is bitstream, needs to check FPGA configuration done */
		if (IsBitstream) {
			debug_xil_printf("Check FPGA config done....\n");

			/* Check FPGA configuration completion */
			Count = MAX_COUNT;

			while ((StatusReg & PCAP_IXR_PCFG_DONE_MASK) == 0) {

				StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);

				Count -= 1;
				if (!Count) {
					debug_xil_printf("FPGA config timed out, %x\r\n",
					    StatusReg);
					return XST_FAILURE;
				}
			}

			debug_xil_printf("FPGA config done %d, %x\r\n",
			    Count, StatusReg);

			WritePcapISR(StatusReg & PCAP_IXR_PCFG_DONE_MASK);
		}

	} else { /* Not NAND/SD */

		Count = MAX_COUNT;
		/* set up the transfer */
		debug_xil_printf("PCAP: Source = %.8x\r\n",
		     ((u32)SourceData | PCAP_LAST_TRANSFER));

		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_SRC_REG),
		    ((u32)SourceData | PCAP_LAST_TRANSFER));

		debug_xil_printf("PCAP: Dest = %.8x\r\n",
		     ((u32)DestinationData | PCAP_LAST_TRANSFER));
		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG),
		    ((u32)DestinationData | PCAP_LAST_TRANSFER));

		debug_xil_printf("PCAP: SourceLength = %.8x\r\n", SourceLength);
		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_LEN_REG),
		    SourceLength);

		debug_xil_printf("PCAP: DestinationLength = %.8x\r\n",
		     DestinationLength);
		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_LEN_REG),
		    DestinationLength);

		debug_xil_printf("PCAP: polling for completion....\r\n");

		/* Poll for completion */
		StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);

		while((StatusReg & PCAP_IXR_DMA_DONE_MASK) == 0) {

			if ((StatusReg & PCAP_IXR_ERROR_FLAGS_MASK) != 0) {
				debug_xil_printf("FAILURE, PCAP_ISR = 0x%.8x\r\n",
						     StatusReg);

				debug_xil_printf("FAILURE, PCAP_DEBUG_XFER_WRITE_"
						    "COUNT_OFFSET = 0x%.8x\r\n",
				   			PcapIn32(XPSS_DEV_CFG_APB_BASEADDR +
						        PCAP_DEBUG_XFER_WRITE_COUNT_OFFSET));

				debug_xil_printf("FAILURE, PCAP_DEBUG_XFER_READ_"
						    "COUNT_OFFSET = 0x%.8x\r\n",
						    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR +
						        PCAP_DEBUG_XFER_READ_COUNT_OFFSET));

				return XST_FAILURE;
			}

			StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);

			Count -= 1;
			if (!Count) {
				debug_xil_printf("Pcap transfer timed out\r\n");
				return XST_FAILURE;
			}

			if (Count > (MAX_COUNT - 100)) {
				debug_xil_printf("Count for pcap download %d %x\r\n",
				    Count, PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR));
			}
		}

		debug_xil_printf("DMA transfer is done\r\n");

		/* Check FPGA configuration completion */
		if (IsBitstream) {
			Count = MAX_COUNT;

			while ((StatusReg & PCAP_IXR_PCFG_DONE_MASK) == 0) {

				StatusReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR);

				Count -= 1;
				if (!Count) {
					debug_xil_printf("FPGA config timed out\r\n");
					return XST_FAILURE;
				}
			}

			debug_xil_printf("FPGA config done %d\r\n", Count);
		}

		/* clear out the status */
		WritePcapISR(StatusReg & PCAP_IXR_DMA_DONE_MASK);
	}

	/* If non-secure mode, recover MCTRL register to not loopback */
	if ((NonSecure) && (!IsBitstream)) {

		PcapReg = PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG);
		PcapReg &= ~PCAP_MCTRL_DMAQ_LOOPBACK_MASK;

		PcapOut32((XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG),
		           PcapReg);
	}

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function prints PCAP register status.
*
* @param    none
*
* @return   none
*
* @note     none
*
****************************************************************************/

void pcap_register_dump (void) {

	debug_xil_printf("PCAP register dump:\r\n");

	debug_xil_printf("PCAP CTRL %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONTROL_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONTROL_REG));
	debug_xil_printf("PCAP LOCK %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_LOCK_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_LOCK_REG));
	debug_xil_printf("PCAP CONFIG %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONFIG_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_CONFIG_REG));
	debug_xil_printf("PCAP ISR %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ISR));
	debug_xil_printf("PCAP IMR %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_IMR,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_IMR));
	debug_xil_printf("PCAP STATUS %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG));
	debug_xil_printf("PCAP DMA SRC %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_SRC_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_SRC_REG));
	debug_xil_printf("PCAP DMA DEST %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_DMA_DEST_REG));
	debug_xil_printf("PCAP ROM SHADOW CTRL %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_ROM_SHADOW_CRTL,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_ROM_SHADOW_CRTL));
	debug_xil_printf("PCAP MBOOT %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_MBOOT_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_MBOOT_REG));
	debug_xil_printf("PCAP SW ID %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_SW_ID_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_SW_ID_REG));
	debug_xil_printf("PCAP UNLOCK %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_UNLOCK_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_UNLOCK_REG));
	debug_xil_printf("PCAP MCTRL %x: %08x\r\n",
	    XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG,
	    PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_MCTRL_REG));
}

/******************************************************************************/
/**
*
* This function reads the PCAP status.
*
* @param    none
*
* @return   contents of the PCAP status register.
*
* @note         none
*
****************************************************************************/
u32 ReadPcapStatus(void){

	return PcapIn32(XPSS_DEV_CFG_APB_BASEADDR + PCAP_STATUS_REG);

}
