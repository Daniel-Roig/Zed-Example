/* $Id: mio.c,v 1.1.2.2 2011/11/12 11:59:09 naveenm Exp $ */
/******************************************************************************
*
* (c) Copyright 2010 Xilinx, Inc. All rights reserved.
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
* @file mio.c
*
* Contains code for the MIO functionality.
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

#include "mio.h"

/************************** Constant Definitions *****************************/

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/



/************************** Variable Definitions *****************************/

/******************************************************************************/
/******************************************************************************/
/**
*
* This function initializes the controller for the MIO interface.
* writes the unlock value to the SLCR to allow further changes to
* the MIO pins.
*
* @param    none
*
* @return   Data The status register

* @note    none.
*
****************************************************************************/
u32 InitMio(void){

        u32 Data;

        Data = MioIn32(XPSS_SYS_CTRL_BASEADDR);

        /* unlock the SLCR to allow the MIO pins to be changed */
        MioUnlock();

        /* These registers need to be set to 0 before anything else is set up */

        MioOut32(FPGA_RESET_REG, 0);            //   FPGA_RST_CTRL   FPGA Software Reset Control     R/W

        return Data;

}

/******************************************************************************/
/**
*
* This function reads the requested data from MIO  interface.
*
* @param    Offset into the  data space
*
* @return   Data at the provided offset in the
*
* @note    none.
*
****************************************************************************/
u32 ReadMio(u32 Offset){

        u32 Data;

        Data =  MioIn32(MIO_REG_OFFSET +(Offset));

        return Data;

}

/******************************************************************************/
/**
*
* This function writes the provided data to MIO FLASH interface.
*
* @param    Offset into the MIO data space
*                       Data to be written to the MIO data space
*
* @return   XST_SUCCESS if the write completes correctly
*                       XST_FAILURE if the write fails to completes correctly
*
* @note    none.
*
****************************************************************************/
u32 WriteMio(u32 Offset, u32 Data){

        MioOut32(MIO_REG_OFFSET +(Offset),Data);

        return XST_SUCCESS;

}

/******************************************************************************/
/**
*
* This function handles the MIO initialization. This function is used to
* set the required MIO registers to reset state prior to handoff to
* the user code.
*
* @param    none
*
* @return   none
*
* @note    none.
*
****************************************************************************/
void MioReset(void){

        u32 MioRegAddr;
        u32 PinIndex;

        MioRegAddr = MIO_REG_OFFSET;

        MioUnlock();

        /* return the default value */

        for (PinIndex = 0; PinIndex < MIO_MAX_PIN_NUMBER; PinIndex++) {

                /* set default value */
                MioOut32((MioRegAddr + (PinIndex * 4)),MIO_DEFAULT_MASK);
        }

        MioLock();

        return;

}
