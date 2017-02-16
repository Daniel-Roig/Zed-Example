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
******************************************************************************/
/*****************************************************************************/
/**
* @file xparameters.h
*
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  02/01/09 Initial version,
* </pre>
*
* @note	None.
*
******************************************************************************/
#ifndef _XPARAMETERS_H_
#define _XPARAMETERS_H_

#include "xparameters_pss.h"

#define STDIN_BASEADDRESS		XPAR_XUARTPSS_1_BASEADDR
#define STDOUT_BASEADDRESS		XPAR_XUARTPSS_1_BASEADDR

#define XPAR_DDR2_SDRAM_MEM_BASEADDR	0x00000000
#define XPAR_DDR2_SDRAM_MEM_HIGHADDR	0x00FFFFFF

/* PSS Interrupt controller defines */
#define XPAR_GIC_SINGLE_DEVICE_ID	0
#define XPAR_GIC_CPU_BASEADDR		XPSS_GIC_CPU_BASEADDR
#define XPAR_GIC_DIST_BASEADDR		XPSS_GIC_DIS_BASEADDR
#define XPAR_GIC_NUM_INSTANCES		1
#define XPAR_GIC_ACK_BEFORE		0

/* UART Defines */
#define XPAR_XUARTPSS_0_DEVICE_ID	0
#define XPAR_XUARTPSS_0_BASEADDR	XPSS_UART0_BASEADDR
#define XPAR_XUARTPSS_0_CLOCK_HZ	50000000
#define XPAR_XUARTPSS_0_INTR		59
#define XPAR_XUARTPSS_1_DEVICE_ID	1
#define XPAR_XUARTPSS_1_BASEADDR	XPSS_UART1_BASEADDR
#define XPAR_XUARTPSS_1_CLOCK_HZ	50000000
#define XPAR_XUARTPSS_1_INTR		82
#define XPAR_XUARTPSS_NUM_INSTANCES	2

/*IIC Defines */
#define XPAR_XIICPSS_NUM_INSTANCES	2
#define XPAR_XIICPSS_0_DEVICE_ID	0	/* Device ID for instance */
#define XPAR_XIICPSS_0_BASEADDR		XPSS_I2C0_BASEADDR  /* Device base address */
#define XPAR_XIICPSS_0_CLOCK_HZ		100000000
#define XPAR_XIICPSS_0_INTR		57
#define XPAR_XIICPSS_1_DEVICE_ID	1	/* Device ID for instance */
#define XPAR_XIICPSS_1_BASEADDR		XPSS_I2C1_BASEADDR	/* Device base address */
#define XPAR_XIICPSS_1_CLOCK_HZ		100000000
#define XPAR_XIICPSS_1_INTR		80

/* TTC Defines */
#define XPAR_XTTCPSS_NUM_INSTANCES	6
#define XPAR_XTTCPSS_CLOCK_HZ		781250

#define XPAR_XTTCPSS_0_DEVICE_ID	0
#define XPAR_XTTCPSS_0_BASEADDR		XPSS_TTC0_BASEADDR	/* Device base address */
#define XPAR_XTTCPSS_0_INTR		42
#define XPAR_XTTCPSS_0_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ

#define XPAR_XTTCPSS_1_DEVICE_ID	1
#define XPAR_XTTCPSS_1_BASEADDR		XPSS_TTC0_BASEADDR+4	/* Device base address */
#define XPAR_XTTCPSS_1_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ
#define XPAR_XTTCPSS_1_INTR		43

#define XPAR_XTTCPSS_2_DEVICE_ID	2
#define XPAR_XTTCPSS_2_BASEADDR		XPSS_TTC0_BASEADDR+8   /* Device base address */
#define XPAR_XTTCPSS_2_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ
#define XPAR_XTTCPSS_2_INTR		44

#define XPAR_XTTCPSS_3_DEVICE_ID	3
#define XPAR_XTTCPSS_3_BASEADDR		XPSS_TTC1_BASEADDR  /* Device base address */
#define XPAR_XTTCPSS_3_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ
#define XPAR_XTTCPSS_3_INTR		69

#define XPAR_XTTCPSS_4_DEVICE_ID	4
#define XPAR_XTTCPSS_4_BASEADDR		XPSS_TTC1_BASEADDR+4   /* Device base address */
#define XPAR_XTTCPSS_4_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ
#define XPAR_XTTCPSS_4_INTR		70

#define XPAR_XTTCPSS_5_DEVICE_ID	5
#define XPAR_XTTCPSS_5_BASEADDR		XPSS_TTC1_BASEADDR+8   /* Device base address */
#define XPAR_XTTCPSS_5_CLOCK_HZ		XPAR_XTTCPSS_CLOCK_HZ
#define XPAR_XTTCPSS_5_INTR		71

/* GPIO Defines*/
#define XPAR_XGPIO_NUM_INSTANCES	1
#define XPAR_XGPIO_0_DEVICE_ID		0
#define XPAR_XGPIO_BASEADDR		XPSS_GPIO_BASEADDR

/* GEM Defines*/
#define XPAR_XEMACPSS_NUM_INSTANCES	2
#define XPAR_XEMACPSS_0_DEVICE_ID	0
#define XPAR_XEMACPSS_0_BASEADDR	XPSS_GEM0_BASEADDR
#define XPAR_XEMACPSS_1_DEVICE_ID	1
#define XPAR_XEMACPSS_1_BASEADDR	XPSS_GEM1_BASEADDR

/* SPI Defines */
#define XPAR_XSPIPSS_NUM_INSTANCES	2
#define XPAR_SPIPSS_0_DEVICE_ID		0	/* Device ID for instance */
#define XPAR_SPIPSS_0_BASEADDR		XPSS_SPI0_BASEADDR
#define XPAR_SPIPSS_0_CLOCK_HZ		50000000
#define XPAR_SPIPSS_0_INTR		58

#define XPAR_SPIPSS_1_DEVICE_ID		1	/* Device ID for instance */
#define XPAR_SPIPSS_1_BASEADDR		XPSS_SPI1_BASEADDR
#define XPAR_SPIPSS_1_CLOCK_HZ		50000000
#define XPAR_SPIPSS_1_INTR		81

/* WDT Defines*/
#define XPSS_WDT_NUM_INSTANCES		1
#define XPSS_WDT_DEVICE_ID		0

/******************************************************************/
#define XPAR_CPU_CORTEXA9_CORE_CLOCK_FREQ_HZ	12500000
/******************************************************************/
#endif /*_XPARAMETERS_PSS_H_*/
