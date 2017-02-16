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
* @file asm_vectors.s
*
* This file contains the initial vector table for the Zynq ARM A9 processor
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- ---------------------------------------------------
* 1.00a ecm  06/20/08 Initial version
* </pre>
*
* @note
*
* None.
*
******************************************************************************/
.org 0
.text

.global  ErrorLockdown
.global _boot
.global _vector_table
.global __stack



.set SYS_stack,         __stack

.section .vectors
_vector_table:
        b       _boot 
        b       Undef_Handler
        b       Svc_Handler
        b       PrefAbort_Handler 
        b       DataAbort_Handler
		nop
		b       Irq_Handler
        b       Fiq_Handler
    
Loop:   b       Loop
        
Undef_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A301                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown

Svc_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A302                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown

PrefAbort_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A303                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown

DataAbort_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A304                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown

Irq_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A305                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown

Fiq_Handler:
        mrs     r0, cpsr                        /* Read CPSR to r0 */
        mvn     r1, #0x1f                       /* put 0xFFFFFFE0 to r1 */
        and     r2, r1, r0
        orr     r2, r2, #0x1F                   /* CPSR to in sys mode */
        msr     cpsr, r2                        /* update CPSR */
        ldr     r13,=SYS_stack                  /* SYS stack pointer */

        ldr     r0, =0x0000A306                 /* EXCEPTION_TAKEN_FAIL */
        
        b       ErrorLockdown


.end
