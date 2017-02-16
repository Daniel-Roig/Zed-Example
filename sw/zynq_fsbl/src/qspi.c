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
* @file qspi.c
*
* Contains code for the QSPI FLASH functionality.
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

#include "qspi.h"
#include "mio.h"
#include "image_mover.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define QspiRegIn32             Xil_In32
#define QspiRegOut32            Xil_Out32
/************************** Function Prototypes ******************************/
static u32 ReadQspi(u32 Address);

/************************** Variable Definitions *****************************/

extern u32 FlashReadBaseAddress;

/******************************************************************************/
/******************************************************************************/
/**
*
* This function initializes the controller for the QSPI interface.
*
* @param    none
*
* @return   XST_SUCCESS if the controller initializes correctly
*           XST_FAILURE if the controller fails to initializes correctly
*
* @note    none.
*
****************************************************************************/
u32 InitQspi(void){

	/* Change the divisor setting */
	QspiOut32((XPSS_QSPI_BASEADDR + QSPI_CONFIG_OFFSET),
	             QSPI_CONFIG_RESET_AUTO);

	return XST_SUCCESS;
}

/******************************************************************************/
/**
*
* This function reads the requested data from QSPI FLASH interface.
*
* @param    Offset into the FLASH data space
*
* @return   Data at the provided offset in the FLASH
*
* @note    none.
*
****************************************************************************/
static u32 ReadQspi(u32 Address){

	return QspiRegIn32(Address);

}

/******************************************************************************/
/**
*
* This function provides the QSPI FLASH interface for the Simplified header
* functionality.
*
* @param    SourceAddress is address in FLASH data space
*           DestinationAddress is address in OCM data space
*
* @return   XST_SUCCESS if the write completes correctly
*           XST_FAILURE if the write fails to completes correctly
*
* @note    none.
*
****************************************************************************/

u32 QspiAccess( u32 SourceAddress, u32 DestinationAddress, u32 LengthBytes)
{

	u32 Data;
	u32 Count;
	u32 *SourceAddr;
	u32 *DestAddr;

	/* check for non-word tail, add bytes to cover the end */
	if ((LengthBytes%4) != 0){
		LengthBytes += (4 - (LengthBytes & 0x00000003));
	}


	SourceAddr = (u32 *)(SourceAddress + FlashReadBaseAddress);
	DestAddr = (u32 *)(DestinationAddress);

	/* Word transfers, endianism isn't an  issue */
	for (Count=0; Count < (LengthBytes / 4); Count++){
 
		Data = ReadQspi((u32)(SourceAddr++));
		Xil_Out32((u32)(DestAddr++), Data);
	}

	return XST_SUCCESS;

}

