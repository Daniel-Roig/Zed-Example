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
* @file main.c
*
* Reference implementation for for the First Stage Boot Loader (FSBL).
*
* For complete description of FSBL, please refer to FSBL PDD document.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a jz  06/04/11   Initial release
*
* </pre>
*
* @note
*
******************************************************************************/

/***************************** Include Files *********************************/

#include "fsbl.h"
#include "mio.h"
#include "qspi.h"
#include "nand.h"
#include "nor.h"
#include "sd.h"
#include "pcap.h"

#include "image_mover.h"

#include "xil_printf.h"

#ifndef PEEP_CODE
#include "ps7_init.h"
#endif

/************************** Constant Definitions *****************************/
#define DEBUG_DECRYPTION 0
//#define ENABLE_ECC
#define ZED_REV_B

/* Watchdog Timer defines */
#define WDT_ZERO_MODE_OFFSET       0x0
#define WDT_CTR_CNTRL_OFFSET       0x4
#define WDT_RESTART_OFFSET         0x8
#define WDT_STS_OFFSET             0xC

#define WDT_ENABLE                 0x1
#define WDT_RST_ENABLE             0x2
#define WDT_IRQ_ENABLE             0x4
#define WDT_ZERO_KEY               0x00ABC000
#define WDT_ZERO_ENABLE_VAL   (WDT_ENABLE | WDT_RST_ENABLE)
#define WDT_USER_SETTING_MASK      0xF
//#define WDT_CTR_CNTRL_VAL          ((0x248 << 14) | 0x0F)
#define WDT_CTR_CNTRL_VAL          ((0x248 << 6) | 0xFFFF)
#define WDT_RESTART_VAL            0x00001999
#define WDT_RESET_VAL              0x1

/* SLCR DDR control register and define */
#define DDR_ECC_SCRUB_MODE_REG      (XPSS_DDR_CTRL_BASEADDR + 0xF4)
#define DDR_ECC_ENABLE_MASK         0x4  

/* Remove once PCW supports peripheral initialization */
#undef UART_DEVICE_ID
#define UART_DEVICE_ID XPAR_XUARTPSS_0_DEVICE_ID

/* Status UART MUX defines defines */
#define STATUS_UART_MUX_REG             (XPSS_SYS_CTRL_BASEADDR + 0x500)
#define STATUS_UART_MUX_OUTPUT_MASK            (0x00000001)

/* Reboot status register defines:
 * 0xF0000000 for FSBL fallback mask to notify Boot Rom 
 * 0x60000000 for FSBL to mark that FSBL has not handoff yet
 * 0x0F000000 for FSBL to record partition number to work on
 * 0x00FFFFFF for user application to use across soft reset
 */
#define FSBL_FAIL_MASK          0xF0000000
#define FSBL_IN_MASK            0x60000000
#define PARTITION_NUMBER_MASK   0x0F000000
#define REBOOT_STATUS_NOTUSED   0x00FFFFFF  /* Not touched */ 

#define PARTITION_NUMBER_SHIFT          24
#define MAX_PARTITION_NUMBER            0xE

/* Word-alignment check */
#define WORD_ALIGN_MASK                 0x3

/* The address that holds the base address for the image Boot ROM found */
#define BASEADDR_HOLDER              0xFFFFFFF8

/**************************** Type Definitions *******************************/
/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/
extern void FsblHandoffExit(u32 FsblStartAddr);
extern void FsblHandoffJtagExit(void);

extern void InitUART(void);
static void FsblHandoff(u32 FsblStartAddr);

static void RestartWDT(void);
static void EnableWDT(void);
static void DisableWDT(void);
static int IsWDTReset(void);

#ifdef ENABLE_ECC 
static void DDREccEnable(void);
static int DDREccEnabled(void);
static void DDREccDisable(void);
#endif

static void MarkFSBLIn(void);
static void ClearFSBLIn(void);
extern void init_ddr(void);

/************************** Variable Definitions *****************************/
int SkipPartition;

/* Base Address for the Read Functionality for Image Processing */
u32 FlashReadBaseAddress = 0;

extern ImageMoverType MoveImage;

/*****************************************************************************/
/**
*
* This is the main function for the FSBL ROM code.
*
* The functionality progresses as follows:
*
* @param    None.
*
* @return   XST_SUCCESS to indicate success, otherwise XST_FAILURE.
*
* @note     None.
*
****************************************************************************/
int main(void)
{
	u32 BootModeRegister = 0;
	u32 Status = 0;
	u32 HandoffAddress;
	volatile u32 RebootStatusRegister = 0;
	u32 ImageStartAddress = 0;
	u32 MediaAddr;
	int PartitionNumber = 0;

	BootModeRegister = FsblIn32(BOOT_MODE_REG);

#if 1
	/* Allow changes to the MIO pins */
	(void)InitMio();
#endif

#ifdef PEEP_CODE
	/* initialize the DDR for application code */
	init_ddr();
#else
	ps7_config(ps7_mio_init_data);
	ps7_config(ps7_pll_init_data);
	ps7_config(ps7_clock_init_data);
	ps7_config(ps7_ddr_init_data);
#endif

	/* Unlock MIO for register access inside FSBL */
	MioUnlock();

	InitUART();

	debug_xil_printf("Finished init UART\r\n");

	/* Check WDT reset, if so, fallback */
	if (IsWDTReset()) {
		xil_printf("WDT reset happened, falling back...\n\r");
		OutputStatus(FSBL_HANGS);
		FsblFallback();	

	} else {

		MarkFSBLIn();
	}

	RestartWDT();
	EnableWDT();
	PatWDT();

	/* This code need to be removed after proper time out value */
	DisableWDT();
	debug_xil_printf("WDT disabled \r\n");

	debug_xil_printf("There should not be any DDR prints\n");
	BootModeRegister &= BOOT_MODES_MASK;

	debug_xil_printf("Boot mode %x\n\r", BootModeRegister);

	if (BootModeRegister == QSPI_MODE) {
		Status = InitQspi();
		if (Status == XST_FAILURE){
			/* Error Handling here */
			debug_xil_printf("QSPI initialization error\r\n");
			OutputStatus(PERIPHERAL_INIT_FAIL);
			FsblFallback();	
		}
		/* QSPI has been initialized by Boot ROM at the highest speed
		 * We do not to do anything about it */
		MoveImage = QspiAccess;
		FlashReadBaseAddress = XPSS_QSPI_LIN_BASEADDR;
		debug_xil_printf("FSBL QSPI %x\r\n", FlashReadBaseAddress);
	} else

	if (BootModeRegister == NAND_FLASH_MODE) {
		/* Boot ROM always initialize the nand at lower speed
		 * This is the chance to put it to an optimum speed for your nand
		 * device
		 */ 
		debug_xil_printf("To init NAND\n\r");
		Status = InitNand();
		if (Status == XST_FAILURE){
			/* Error Handling here */
			debug_xil_printf("NAND initialization error\r\n");
			OutputStatus(PERIPHERAL_INIT_FAIL);
			FsblFallback();	
		}
		MoveImage = NandAccess;
		debug_xil_printf("FSBL NAND\r\n");
	} else

	if (BootModeRegister == NOR_FLASH_MODE) {

		debug_xil_printf("To init NOR\r\n");
		/* Boot ROM always initialize the nor at lower speed
		 * This is the chance to put it to an optimum speed for your nor
		 * device
		 */ 
		Status = InitNor();
		debug_xil_printf("Finished init NOR %d\r\n", Status);
		if (Status == XST_FAILURE){
			/* Error Handling here */
			debug_xil_printf("NOR initialization error\r\n");
			OutputStatus(PERIPHERAL_INIT_FAIL);
			FsblFallback();	
		}
		MoveImage = NorAccess;
		debug_xil_printf("FSBL NOR\r\n");
	} else

	if (BootModeRegister == SD_MODE) {

		Status = InitSD("BOOT.BIN");
		if (Status != XST_SUCCESS){
        	debug_xil_printf("InitSD failed\r\n");
			OutputStatus(PERIPHERAL_INIT_FAIL);
			FsblFallback();	
		}
		MoveImage = SDAccess;
		debug_xil_printf("FSBL SD %x\r\n", FlashReadBaseAddress);
	} else

	if (BootModeRegister == JTAG_MODE) {
		debug_xil_printf("JTAG Mode\r\n");
		DisableWDT();
		/* Clear our mark in reboot status register */
		ClearFSBLIn();
		FsblHandoffJtagExit();
	} else {

		debug_xil_printf("Unknown Mode %d\r\n", BootModeRegister);

		OutputStatus(ILLEGAL_BOOT_MODE);
		/* fallback fsbl starts, no return */
		FsblFallback();
	}

	debug_xil_printf("read base %x\r\n", FlashReadBaseAddress);

	MediaAddr = FlashReadBaseAddress &  MEDIA_ADDRESS_MASK;

	if ((MediaAddr != XPSS_QSPI_LIN_BASEADDR) &&
	    (MediaAddr != XPSS_PARPORT0_BASEADDR) &&
	    (MediaAddr != XPSS_PARPORT1_BASEADDR) &&
	    (MediaAddr != XPSS_NAND_BASEADDR)     &&
	    (FlashReadBaseAddress != XPSS_SDIO0_BASEADDR) &&
	    (FlashReadBaseAddress != XPSS_SDIO1_BASEADDR))  {

		debug_xil_printf("Invalid flash address %x\r\n", FlashReadBaseAddress);

		OutputStatus(INVALID_FLASH_ADDRESS);
		FsblFallback();	
	}

//#ifdef ENABLE_ECC 
//	/* Enable ECC for DDR */
//	DDREccEnable();
//
//	debug_xil_printf("After enabled ECC ------- \n\r");
//	debug_xil_printf("After enabled ECC ------- \n\r");
//	debug_xil_printf("After enabled ECC ------- \n\r");
//	debug_xil_printf("After enabled ECC ------- \n\r");
//	debug_xil_printf("After enabled ECC ------- \n\r");
//	debug_xil_printf("After enabled ECC ------- \n\r");
//
//#if 0
//	/* Zero out DDR for DT591989, 0x100000 (1M) - 0x8000000 (128M) */
//	{
//		u32 StartAddr = 0x6000000;
//		u32 EndAddr = 0x6040000;
//
//		DisableWDT();
//		debug_xil_printf("To Zero OUT DDR %x -- %x\n\r",
//		    StartAddr, EndAddr);
//
//		memset_rom((void *)StartAddr, 0, (EndAddr - StartAddr));
//
//		debug_xil_printf("Finished Zero OUT DDR %x -- %x\n\r",
//		    StartAddr, EndAddr);
//	
//		RestartWDT();
//		EnableWDT();
//		PatWDT();
//	}
//#endif
//#endif
//
//	{
//#if 1
//	/* Debug of palladium ddr setting hanging on both peep and palladium */
//	u32 *AddrP;
//
//	debug_xil_printf("To write to 0x1000000\r\n");
//
//	AddrP = (u32 *)0x1000000;
//
//	*AddrP = 0x5A5B5C5D;
//	debug_xil_printf("DDR read of written value of 0x5A5B5C5D: %x\r\n",
//		*AddrP);
//
//	debug_xil_printf("To write to 0x6000000\r\n");
//	AddrP = (u32 *)0x6000000;
//
//	*AddrP = 0x5A5B5C5D;
//	debug_xil_printf("DDR read of written value of 0x5A5B5C5D: %x\r\n",
//		*AddrP);
//
//	debug_xil_printf("To write to 0x1000000\r\n");
//
//	AddrP = (u32 *)0x1000000;
//
//	*AddrP = 0x5D5C5B5A;
//	debug_xil_printf("DDR read of written value of 0x5D5C5B5A: %x\r\n",
//		*AddrP);
//#endif
//	}

	debug_xil_printf("DDR prints and access skipped\n");
	/* Partition walk */
	/* If partition is bitstream, then no reset is necessary.
	 * SkipPartition is to the next partition without soft reset
	 */
	SkipPartition = 1; 

	while (SkipPartition) {
		RebootStatusRegister = FsblIn32(REBOOT_STATUS_REG);

		/* Prevent WDT reset */
		PatWDT();

		/* Clear out fallback mask from previous run
		 * We start from the first partition again */
		if ((RebootStatusRegister & FSBL_FAIL_MASK) ==
		     FSBL_FAIL_MASK) {

			debug_xil_printf("Reboot status shows previous run falls back\r\n");

			RebootStatusRegister &= ~(FSBL_FAIL_MASK | PARTITION_NUMBER_MASK);

			FsblOut32(REBOOT_STATUS_REG,
			    RebootStatusRegister & 
			      ~(FSBL_FAIL_MASK | PARTITION_NUMBER_MASK));
		}

		ImageStartAddress = *(u32 *)BASEADDR_HOLDER; 

		PartitionNumber = (RebootStatusRegister & PARTITION_NUMBER_MASK)
		                    >> PARTITION_NUMBER_SHIFT;


		debug_xil_printf("flash read base addr %x, image base %x\r\n",
		    FlashReadBaseAddress, ImageStartAddress);

		SkipPartition = 0;
 
		HandoffAddress = PartitionMove(ImageStartAddress, PartitionNumber);
		debug_xil_printf("Hand off address %x\r\n", HandoffAddress);

		
		if (SkipPartition) {
			debug_xil_printf("FSBL main: Skip partition\r\n");
			continue;
		}

		/* Image move encounters errors, fallback
		 * Error code has been printed in image mover */
		if (HandoffAddress == MOVE_IMAGE_FAIL) {
			debug_xil_printf("FSBL fall back\r\n");

			/* fallback starts, no return */
			FsblFallback();	
		}
	}


#if 0
	RestartWDT();
	PatWDT();
	xil_printf("WDT registers:\n\r");
	xil_printf("CTRL %x, intended %x\n\r", 
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_CTR_CNTRL_OFFSET),
	    WDT_CTR_CNTRL_VAL);


	xil_printf("ZERO_MODE %x, intended %x\n\r", 
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET),
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET) |
	    WDT_ZERO_ENABLE_VAL | WDT_ZERO_KEY);
	xil_printf("STS %x\n\r", 
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_STS_OFFSET));

	xil_printf("Enter infinite loop\n\r");
	while(1);
#endif

#if 1
	debug_xil_printf("To handoff, End of main\r\n");
#endif

#if 1
	/* Disable WDT to prevent WDT reset in application.
	 * User can remove this disable and use PatWDT() to keep WDT
	 * active, so that application hanging can be detected */
	DisableWDT();
#endif

	/* If using SD, close the file */
	if (BootModeRegister == SD_MODE) {
		ReleaseSD();
	}

	/* Clear our mark in reboot status register */
	ClearFSBLIn();

	debug_xil_printf("Before handoff reboot status register %x\r\n",
	    FsblIn32(REBOOT_STATUS_REG));

#if 0
	/* DEBUG, for reboot status register not updating to 0xF000000 */
	FsblFallback();
#endif

	/* Reset the USB */
	{
		debug_xil_printf("Reset USB...\r\n");

		/* Set data dir */
		*(unsigned int *)0xe000a284 = 0x00000001;

		/* Set OEN */
		*(unsigned int *)0xe000a288 = 0x00000001;

		/* Fir REVB Set data value low for reset, then back high */
#ifdef ZED_REV_B
		*(unsigned int *)0xe000a048 = 0x00000000;
		*(unsigned int *)0xe000a048 = 0x00000001;
#else
		*(unsigned int *)0xe000a048 = 0x00000001;
		*(unsigned int *)0xe000a048 = 0x00000000;
#endif
	}

	/* Lock MIO so application cannot mess with control registers */
	MioLock();

	FsblHandoff(HandoffAddress);

	/* Should not return here, if it ever does, it is an error */
	OutputStatus(ILLEGAL_RETURN);
	FsblFallback();
	
	return 0;
}

/******************************************************************************/
/**
*
* This function reset the CPU and goes for Boot ROM fallback handling
*
* @param None
*
* @return
*   None
*
****************************************************************************/
void FsblFallback(void)
{
	u32 RebootStatusReg = FsblIn32(REBOOT_STATUS_REG);

	MioUnlock();

	/* Notify Boot ROM something is wrong */ 
	FsblOut32(REBOOT_STATUS_REG, RebootStatusReg | FSBL_FAIL_MASK);

	xil_printf("FSBL fallback %x\r\n", FsblIn32(REBOOT_STATUS_REG));
	xil_printf("FSBL fallback %x\r\n", FsblIn32(REBOOT_STATUS_REG));

	/* Barrier */
	asm(
		"dsb\n\t"
		"isb"
	);

	/* Hand off to Boot ROM */
	MioUnlock();

	/* Reset PSS, so Boot ROM will restart */
	FsblOut32(PSS_RST_CTRL_REG, PSS_RST_MASK);

	MioLock();
}

/******************************************************************************/
/**
* This function returns the encryption status of the image
*
* @param None
*
* @return
*   Encryption type, to be used for the PCAP call
*
****************************************************************************/
u32 FsblGetEncryptType(void)
{
	u32 ImageStart;
	u32 EncryptType;	

	ImageStart = *(u32 *)BASEADDR_HOLDER; 
	
	MoveImage(ImageStart + IMAGE_ENC_FLAG_OFFSET,
	   (u32)&EncryptType, 4);


	if ((EncryptType != IMAGE_ENCRYPTED_EFUSE) &&
	    (EncryptType != IMAGE_ENCRYPTED_BRAM)) {

		return IMAGE_ENCRYPTED_NONE;

	} else {

		return EncryptType;
	} 
}

/******************************************************************************/
/**
* This function set the address for the next partition
*
* @param Num is the partition number for the next partition, it should be less
*        than 15
*
* @return
*   . XST_SUCCESS if everything works
*   . XST_INVALID_PARAM if partition number is more than 15 
*
****************************************************************************/
int FsblSetNextPartition(int Num)
{
	u32 RebootStatusReg;
	u32 ResetReg;

	/* Address must be word-aligned */
	if (Num > MAX_PARTITION_NUMBER) {
		debug_xil_printf("Partition number %d too large, limit %d \n",
		    Num, MAX_PARTITION_NUMBER);

		/* Let the caller decide what to do next
		 * If they continue with its partition walk, the last partition
		 * will be repeated */
		return XST_INVALID_PARAM;
	}

	RebootStatusReg = FsblIn32(REBOOT_STATUS_REG);
	ResetReg = RebootStatusReg & ~PARTITION_NUMBER_MASK;

	FsblOut32(REBOOT_STATUS_REG, ResetReg |
	     (Num << PARTITION_NUMBER_SHIFT));

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function hands the A9/PSS off to the loaded user code.
*
* @param    none
*
* @return   none
*
* @note
*   This function does not return.
*
****************************************************************************/
void FsblHandoff(u32 FsblStartAddr){

	OutputStatus(SUCCESSFUL_HANDOFF);

	FsblHandoffExit(FsblStartAddr);

	OutputStatus(ILLEGAL_RETURN);

	FsblFallback();	
}

/******************************************************************************/
/**
*
* This function outputs the status for the provided State in the boot process.
*
* @param    State - where in the boot process the output is desired.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
void OutputStatus(u32 State){

	volatile u32 Status;

	debug_xil_printf("FSBLStatus = 0x%.4x\r\n", State);

	Status = FsblIn32(STDOUT_BASEADDRESS + XUARTPSS_SR_OFFSET);

	while ((Status & XUARTPSS_SR_TXEMPTY) != XUARTPSS_SR_TXEMPTY){
		Status = FsblIn32(STDOUT_BASEADDRESS + XUARTPSS_SR_OFFSET);
	}

}

/******************************************************************************/
/**
*
* This function handles the error and lockdown processing and outputs the status
* for the provided State in the boot process.
*
* This function is called upon exceptions.
*
* @param    State - where in the boot process the error occured.
*
* @return   None.
*
* @note    This function does not return, the PSS block is reset 
*
****************************************************************************/
void ErrorLockdown(u32 State){

	xil_printf("Error lock down %x\r\n", State);
	OutputStatus(EXCEPTION_TAKEN_FAIL);

	FsblFallback();	

}

/******************************************************************************/
/**
*
* This function sets a memory region with a set value 
*
* @param s is the starting address 
* @param c is the value to set to
* @param n is the number of bytes to set the value 
*
* @return
*   Starting address of memory region
*
****************************************************************************/
void *(memset_rom)(void *s, int c, u32 n)
{
	unsigned char *us = s;
	unsigned char uc = c;
	while (n-- != 0)
		*us++ = uc;
	return s;
}

/******************************************************************************/
/**
*
* This function copies a memory region to another memory region 
*
* @param s1 is starting address for destination
* @param s2 is starting address for the source
* @param n is the number of bytes to copy 
*
* @return
*   Starting address for destination
*
****************************************************************************/
void *(memcpy_rom)(void * s1, const void * s2, u32 n)
{
	char *dst = s1;
	const char *src = s2;

	/* Loop and copy.  */
	while (n-- != 0)
		*dst++ = *src++;
	return s1;
}

/******************************************************************************/
/**
*
* This function copies a string to another, the source string must be null-
* terminated. 
*
* @param dest is starting address for the destination string
* @param src is starting address for the source string
*
* @return
*   Starting address for the destination string 
*
****************************************************************************/
char *strcpy_rom(char *dest, const char *src)
{
	unsigned i;
	for (i=0; src[i] != '\0'; ++i)
		dest[i] = src[i];
	dest[i] = '\0';
	return dest;
}

/******************************************************************************/
/**
*
* This function reset the WDT's reset interval to avoid immediate reset  
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
static void RestartWDT(void) {
	FsblOut32(XPSS_WDT_BASEADDR + WDT_RESTART_OFFSET, WDT_RESTART_VAL);
}

/******************************************************************************/
/**
*
* This function enables the WDT to catch misbehaving FSBL 
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
void EnableWDT(void){
	u32 WdtReg = FsblIn32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET);

#if 0
	xil_printf("Before set WDT zero mode %x ctrl %x\n\r",
		WdtReg, 
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_CTR_CNTRL_OFFSET));
#endif

	FsblOut32(XPSS_WDT_BASEADDR + WDT_CTR_CNTRL_OFFSET,
	    WDT_CTR_CNTRL_VAL);

	WdtReg &= ~WDT_USER_SETTING_MASK;
	FsblOut32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET,
	    WdtReg | WDT_ZERO_ENABLE_VAL | WDT_ZERO_KEY);
}

/******************************************************************************/
/**
*
* This function disables the WDT to avoid WDT reset in application code 
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/

void DisableWDT(void){
	u32 WdtReg = FsblIn32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET);

	FsblOut32(XPSS_WDT_BASEADDR + WDT_ZERO_MODE_OFFSET,
	    (WdtReg | WDT_ZERO_KEY) & ~WDT_ENABLE);

}

/******************************************************************************/
/**
*
* This function restarts the WDT to avoid WDT reset 
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/

void PatWDT(void){

	FsblOut32(XPSS_WDT_BASEADDR + WDT_RESTART_OFFSET, WDT_RESTART_VAL);
}

/******************************************************************************/
/**
*
* This function checks whether WDT reset has happened during FSBL run
*
* If WDT reset happened during FSBL run, then need to fallback
*
* @param    None.
*
* @return 
*    1 -- if WDT reset happens while FSBL is running
*    0 -- otherwise
*
* @note     None.
*
****************************************************************************/
int IsWDTReset(void){
	u32 RegValue;

	RegValue = FsblIn32(REBOOT_STATUS_REG);

#if 1
	debug_xil_printf("Reboot status %x\n\r", RegValue);
	debug_xil_printf("WDT STS %x\n\r", 
	    FsblIn32(XPSS_WDT_BASEADDR + WDT_STS_OFFSET));
#endif

	/* If the FSBL_IN_MASK Has not been cleared, WDT happened
	 * before FSBL exits */
	if ((RegValue & FSBL_FAIL_MASK) == FSBL_IN_MASK) {
		return 1;
	} else {
		return 0;
	}
}

/******************************************************************************/
/**
*
* This function sets FSBL is running mask in reboot status register 
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
void MarkFSBLIn(void) {

	FsblOut32(REBOOT_STATUS_REG,
	    FsblIn32(REBOOT_STATUS_REG) | FSBL_IN_MASK);
}

/******************************************************************************/
/**
*
* This function clears FSBL is running mask in reboot status register 
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
void ClearFSBLIn(void) {

	FsblOut32(REBOOT_STATUS_REG,
	    FsblIn32(REBOOT_STATUS_REG) & ~FSBL_IN_MASK);
}

#ifdef ENABLE_ECC 
/******************************************************************************/
/**
*
* This function enables DDR on-die ECC
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
static void DDREccEnable(void)
{
	debug_xil_printf("To Read in ECC scrub mode reg at %x\r\n",
	    DDR_ECC_SCRUB_MODE_REG);

	debug_xil_printf("Read in ECC scrub mode reg %x\r\n",
		FsblIn32(DDR_ECC_SCRUB_MODE_REG));

	FsblOut32(DDR_ECC_SCRUB_MODE_REG,
	    FsblIn32(DDR_ECC_SCRUB_MODE_REG) | DDR_ECC_ENABLE_MASK); 
}

/******************************************************************************/
/**
*
* This function checks whether DDR on-die ECC is enabled
*
* @param    None.
*
* @return
*    1 -- DDR on-die ECC is enabled
*    0 -- otherwise
*
* @note     None.
*
****************************************************************************/
static int DDREccEnabled(void)
{
	if(FsblIn32(DDR_ECC_SCRUB_MODE_REG) | DDR_ECC_ENABLE_MASK)
		return 1;
	else
		return 0;
}

/******************************************************************************/
/**
*
* This function disables DDR on-die ECC
*
* @param    None.
*
* @return   None.
*
* @note     None.
*
****************************************************************************/
static void DDREccDisable(void)
{
	FsblOut32(DDR_ECC_SCRUB_MODE_REG,
	    FsblIn32(DDR_ECC_SCRUB_MODE_REG) & ~DDR_ECC_ENABLE_MASK); 
}

#endif
