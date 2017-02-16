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
* @file translation_table.s
*
* This file contains the initialization for the MMU table in RAM
* needed by the ARM1176 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  06/20/08 Initial version
* 2.00a jz   05/20/11 Minor cleanup
*
* </pre>
*
* @note
*
* None.
*
******************************************************************************/
	.globl  MMUTable

	.section .mmu_tbl,"aS"

MMUTable:
/* Each table entry occupies one 32-bit word and there are
 * 4096 entries, so the entire table takes up 16KB.
 *  Each entry covers a 1MB section.
 *
 *  Value for the MMU table entry:
 * 	 . lowest nibble: if 0, not accessible; 2, not cacheable nor bufferable
 *         a, cacheable and bufferable 
 *    . second lowest nibble: always 0
 *    . third lowest nibble: (AP) c, read/write
 */

.set SECT,  0                  

/* DDR */
.word   SECT + 0xc0a            /* TEX=b000 AP=b11, Domain=b0, C=b1, B=b0 */

.set    SECT, SECT+0x100000

.rept   0x003FF                 /* 0x00100000 - 0x3FFFFFFF */
.word   SECT + 0xc0a            /* TEX=b000 AP=b11, Domain=b0, C=b1, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* unassigned memory causes fault if accessed.*/
.rept   0x00400                 /* 0x40000000 - 0x7FFFFFFF  */
.word   SECT + 0x000            /* TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

.rept   0x00100                 /* 0x80000000 - 0x8FFFFFFF FPGA space DT621135 */       
.word   SECT + 0x000            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

.rept   0x00500                /* 0x90000000 - 0xDFFFFFFF  fault if accessed */
.word   SECT + 0x000           /* TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* 0xE0000000 - 0xE00FFFFF UART, SMC,Quad-SPI controllers address */
.word   SECT + 0xc02            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000

.rept   0x0000F                 /* 0xE0100000 - 0xE0FFFFFF  SDIO */
.word   SECT + 0xc02            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr 

/* NAND */
.rept   0x00010                 /* 0xE1000000 - 0xE10FFFFFF  */
.word   SECT + 0xc02            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr 

/* NOR memory, allows Execute in place */
.rept   0x00040                 /* 0xE2000000 - 0xE5FFFFFF  */
.word   SECT + 0xc0a            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* Peripherals and unassigned, fault if accessed. */
.rept   0x00120                 /* 0xE6000000 - 0xF7FFFFFF  */
.word   SECT + 0x000            /* TEX=b000 AP=b11, Domain=b0, C=b1, B=b1 */
.set    SECT, SECT+0x100000
.endr

/* SLCR */
.rept   0x00010                 /* 0xF8000000 - 0xF8FFFFFF  */
.word   SECT + 0xc02            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* Peripherals and unassigned, fault if accessed. */
.rept   0x00030                 /* 0xF9000000 - 0xFBFFFFFF  */
.word   SECT + 0x000            /* TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* Quad-SPI memory, allows Execute in place */
.rept   0x00010                 /* 0xFC000000 - 0xFCFFFFFF  */
.word   SECT + 0xc0a            /* TEX=b000 AP=b01, Domain=b0, C=b1, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* Peripherals and unassigned, fault if accessed */
.rept   0x0002F                 /* 0xFD000000 - 0xFFEFFFFF  */
.word   SECT + 0x000            /* TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 */
.set    SECT, SECT+0x100000
.endr

/* upper address range for OCM RAM, used in handoff. */
.word   SECT + 0xc0a            /* TEX=b000 AP=b11, Domain=b0, C=b0, B=b0 */

.end
