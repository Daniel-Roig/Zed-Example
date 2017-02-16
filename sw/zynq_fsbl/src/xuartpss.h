/*****************************************************************************
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
*****************************************************************************/
/****************************************************************************/
/**
*
* @file xuartpss.h
*
* This driver supports the following features:
*
* - Dynamic data format (baud rate, data bits, stop bits, parity)
* - Polled mode
* - Interrupt driven mode
* - Transmit and receive FIFOs (32 byte FIFO depth)
* - Access to the external modem control lines
*
* <b>Initialization & Configuration</b>
*
* The XUartPss_Config structure is used by the driver to configure itself.
* Fields inside this structure are properties of XUartPss based on its hardware
* build.
*
* To support multiple runtime loading and initialization strategies employed
* by various operating systems, the driver instance can be initialized in the
* following way:
*
*   - XUartPss_CfgInitialize(InstancePtr, CfgPtr, EffectiveAddr) - Uses a
*     configuration structure provided by the caller. If running in a system
*     with address translation, the parameter EffectiveAddr should be the
* 	  virtual address.
*
* <b>Baud Rate</b>
*
* The UART has an internal baud rate generator, which furnishes the baud rate
* clock for both the receiver and the transmitter. Ther input clock frequency
* can be either the master clock or the master clock divided by 8, configured
* through the mode register.
*
* Accompanied with the baud rate divider register, the baud rate is determined
* by:
* <pre>
*	baud_rate = input_clock / (bgen * (bdiv + 1)
* </pre>
* where bgen is the value of the baud rate generator, and bdiv is the value of
* baud rate divider.
*
* <b>Interrupts</b>
*
* The FIFOs are not flushed when the driver is initialized, but a function is
* provided to allow the user to reset the FIFOs if desired.
*
* The driver defaults to no interrupts at initialization such that interrupts
* must be enabled if desired. An interrupt is generated for one of the
* following conditions.
*
* - A change in the modem signals
* - Data in the receive FIFO for a configuable time without receiver activity
* - A parity error
* - A framing error
* - An overrun error
* - Transmit FIFO is full
* - Transmit FIFO is empty
* - Receive FIFO is full
* - Receive FIFO is empty
* - Data in the receive FIFO equal to the receive threshold
*
* The application can control which interrupts are enabled using the
* XUartPss_SetInterruptMask() function.
*
* In order to use interrupts, it is necessary for the user to connect the
* driver interrupt handler, XUartPss_InterruptHandler(), to the interrupt
* system of the application. A separate handler should be provided by the
* application to communicate with the interrupt system, and conduct
* application specific interrupt handling. An application registers its own
* handler through the XUartPss_SetHandler() function.
*
* <b>Data Transfer</b>
*
* The functions, XUartPss_Send() and XUartPss_Recv(), are provided in the
* driver to allow data to be sent and received. They can be used in either
* polled or interrupt mode.
*
* @note
*
* The default configuration for the UART after initialization is:
*
* - 9,600 bps or XPAR_DFT_BAUDRATE if defined
* - 8 data bits
* - 1 stop bit
* - no parity
* - FIFO's are enabled with a receive threshold of 8 bytes
* - The RX timeout is enabled with a timeout of 1 (4 char times)
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  	Date     Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	drg/jz 05/12/08 First Release
* </pre>
*
*****************************************************************************/

#ifndef XUARTPSS_H		/* prevent circular inclusions */
#define XUARTPSS_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files ********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xstatus.h"
#include "xuartpss_hw.h"

/************************** Constant Definitions ****************************/

/*
 * The following constants indicate the max and min baud rates and these
 * numbers are based only on the testing that has been done. The hardware
 * is capable of other baud rates.
 */
#define XUARTPSS_MAX_RATE     115200
#define XUARTPSS_MIN_RATE     110

#define XUARTPSS_DFT_BAUDRATE  19200   /* Default baud rate */

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
