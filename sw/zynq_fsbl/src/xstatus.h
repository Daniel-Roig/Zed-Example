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
* @file xstatus.h
*
* This file contains Xilinx software status codes.  Status codes have their
* own data type called int.  These codes are used throughout the Xilinx
* device drivers.
*
******************************************************************************/

#ifndef XSTATUS_H		/* prevent circular inclusions */
#define XSTATUS_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"

/************************** Constant Definitions *****************************/

/*********************** Common statuses 0 - 500 *****************************/

#define XST_SUCCESS			0L
#define XST_FAILURE			1L
#define XST_DEVICE_NOT_FOUND		2L
#define XST_DEVICE_BLOCK_NOT_FOUND	3L
#define XST_INVALID_VERSION		4L
#define XST_DEVICE_IS_STARTED		5L
#define XST_DEVICE_IS_STOPPED		6L
#define XST_FIFO_ERROR			7L	/* an error occurred during an
						   operation with a FIFO such as
						   an underrun or overrun, this
						   error requires the device to
						   be reset */
#define XST_RESET_ERROR			8L	/* an error occurred which
						   requires the device to be
						   reset */
#define XST_DMA_ERROR			9L	/* a DMA error occurred, this
						   error typically requires the
						   device using the DMA to be
						   reset */
#define XST_NOT_POLLED			10L	/* the device is not configured
						   for polled mode operation */
#define XST_FIFO_NO_ROOM		11L	/* a FIFO did not have room to
						   put the specified data into
						   */
#define XST_BUFFER_TOO_SMALL		12L	/* the buffer is not large
						   enough to hold the expected
						   data */
#define XST_NO_DATA			13L	/* there was no data available
						   */
#define XST_REGISTER_ERROR		14L	/* a register did not contain
						   the expected value */
#define XST_INVALID_PARAM		15L	/* an invalid parameter was
						   passed into the function */
#define XST_NOT_SGDMA			16L	/* the device is not configured
						   for scatter-gather DMA
						   operation */
#define XST_LOOPBACK_ERROR		17L	/* a loopback test failed */
#define XST_NO_CALLBACK			18L	/* a callback has not yet been
						   registered */
#define XST_NO_FEATURE			19L	/* device is not configured with
						   the requested feature */
#define XST_NOT_INTERRUPT		20L	/* device is not configured for
						   interrupt mode operation */
#define XST_DEVICE_BUSY			21L	/* device is busy */
#define XST_ERROR_COUNT_MAX		22L	/* the error counters of a
						   device have maxed out */
#define XST_IS_STARTED			23L	/* used when part of device is
						   already started i.e.
						   sub channel */
#define XST_IS_STOPPED			24L	/* used when part of device is
						   already stopped i.e.
						   sub channel */
#define XST_DATA_LOST			26L	/* driver defined error */
#define XST_RECV_ERROR			27L	/* generic receive error */
#define XST_SEND_ERROR			28L	/* generic transmit error */
#define XST_NOT_ENABLED			29L	/* a requested service is not
						   available because it has not
						   been enabled */

#define XST_NAND_PART_NOT_SUPPORTED 1434Li /* Flash type not supported */

/**************************** Type Definitions *******************************/

/***************** Macros (Inline Functions) Definitions *********************/

/************************** Function Prototypes ******************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
