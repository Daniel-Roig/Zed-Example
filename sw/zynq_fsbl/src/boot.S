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
* @file boot.s
*
* This file contains the initial startup code for the Zynq ARM A9 processor
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
.global MMUTable
.global _prestart
.global _boot
.global __stack


.set CRValICacMmu, 0b01000000000001    /* Enable ICache and MMU */

.set SMP_HOLD_ADDRESS, 0xFFFFFF00      /* where the processor is parked */


/* Stack Pointer locations for boot code */

.set SYS_stack,         __stack

.section .boot, "axS"

        /* this initializes the various processor modes */
_prestart:
_boot:

/* only allow cpu0 through */
        bl      invalidate_dcache     /* invalidate dcache */

        mrc     15,0,r0,cr15,cr0,1    /* Fix for  DT565662 */
        orr     r0, r0, #0x40         /* Set the Bit6 in R0 */
        mcr     15,0,r0,cr15,cr0,1 
        mcr     15,0,r0,cr7,cr5,0     /* Invalidate Instruction cache */
        mcr     15,0,r0,cr7,cr5,6     /* Invalidate branch predictor array */

        mcr     15,0,r0,cr8,cr7,0     /* Invalidate entire Unified TLB */

        mrc     p15,0,r11,c0,c0,5
        and     r11, r11, #0xf
        cmp     r11, #0
        beq     OKToRun

LowLoop:        
        wfe
        b       LowLoop
        
OKToRun:        
        /* this is for the performance monitoring */
        mrc     15,0,r0,cr9,cr12,0     /* read control */
        orr     r0, r0,#5
        mcr     15,0,r0,cr9,cr12,0     /* reset cycle counter */
        mrc     15,0,r0,cr9,cr12,0     /* read control */
        orr     r0, r0,#1
        mcr     15,0,r0,cr9,cr12,0     /* enable cycle counter */

        mov     r0, #0x80000000        /* Set C bit */
        mcr     15,0,r0,cr9,cr12,1     /* Write CNTENS Register */  
        
        /* End of peformance monitoring */
                
        mrs     r0, cpsr               /* get the current PSR */
        mvn     r1, #0x1f              /* set up the system stack pointer */
        and     r2, r1, r0
        orr     r2, r2, #0x1F
        msr     cpsr, r2
        ldr     r13,=SYS_stack         /* SYS stack pointer */
                                
        mrc     15, 0, r0, cr2, cr0, 2 /* TTBCR */
        orr     r0, r0, #2             /* set TLB size */
        
        mvn     r0,#0         /* Load MMU domains -- all ones=manager */
        mcr     15,0,r0,cr3,cr0,0

        ldr     r0,=MMUTable           /* Load MMU translation table base */
        mcr     15, 0, r0, cr2, cr0, 0 /* TTB0 */
        mcr     15, 0, r0, cr2, cr0, 1 /* TTB1 */

        ldr     r0,=CRValICacMmu

        mcr     15,0,r0,cr1,cr0,0      /* enable ICache and MMU */
       
        isb                            /* make sure it completes */

        b       _start                 /* jump to C startup code */
        
.Ldone: b       .Ldone                 /* Paranoia: we should never get here */


/*
 *************************************************************************
 *
 * invalidate_dcache - invalidate the entire d-cache by set/way
 *
 * Note: for Cortex-A9, there is no cp instruction for invalidating
 * the whole D-cache. Need to invalidate each line.
 *
 * This code was lifted from the Linux Kernel which lifted it from
 * ARM documentation (p. 1251)
 *
 *************************************************************************
 */

invalidate_dcache:
        dmb                           /* enforce memory accesses to finish */
        mrc     p15, 1, r0, c0, c0, 1 /* read CLIDR */
        ands    r3, r0, #0x7000000
        mov     r3, r3, lsr #23    /* cache level value (naturally aligned) */
        beq     _finished
        mov     r10, #0               /* start with level 0 */
_loop1:
        add     r2, r10, r10, lsr #1  /* work out 3xcachelevel */
        mov     r1, r0, lsr r2 
        and     r1, r1, #7            /* get those 3 bits alone */
        cmp     r1, #2
        blt     _skip                 /* no cache or only i-cache */
        mcr     p15, 2, r10, c0, c0, 0 /* write Cache Size selection register */
        isb                           /* instruction sync */
        mrc     p15, 1, r1, c0, c0, 0 /* reads current Cache Size ID register */

        and     r2, r1, #7            /* extract line length field */
        add     r2, r2, #4    /* add 4 for line length offset (log2 16 bytes) */
        ldr     r4, =0x3ff
        ands    r4, r4, r1, lsr #3    /* r4: max on way size */
        clz     r5, r4              /* r5: bit position of way size increment */
        ldr     r7, =0x7fff
        ands    r7, r7, r1, lsr #13    /* r7: max of index size */
_loop2:
        mov     r9, r4                 /* r9 working copy of max way size */
_loop3:
        orr     r11, r10, r9, lsl r5   /* factor in way number & cache number */
        orr     r11, r11, r7, lsl r2   /* factor in the index number */
        mcr     p15, 0, r11, c7, c6, 2 /* invalidate by set/way */
        subs    r9, r9, #1             /* decrement the way number */
        bge     _loop3
        subs    r7, r7, #1             /* decrement the index */
        bge     _loop2
_skip:
        add     r10, r10, #2           /* increment the cache number */
        cmp     r3, r10
        bgt     _loop1


_finished:
        mov     r10, #0                /* switch back to cache level 0 */
        mcr     p15, 2, r10, c0, c0, 0 /* select current cache level in cssr */
        isb

        bx      lr



.end

