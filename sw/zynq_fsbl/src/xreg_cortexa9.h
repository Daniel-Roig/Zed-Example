/*******************************************************************************
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
*******************************************************************************/
/*****************************************************************************/
/**
*
* @file xreg_cortexa9.h
*
* This header file contains definitions for using inline assembler code. It is
* written specifically for the GNU compiler.
*
* All of the ARM Cortex A9 GPRs, SPRs, and Debug Registers are defined along
* with the positions of the bits within the registers.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who      Date     Changes
* ----- -------- -------- -----------------------------------------------
* 1.00a ecm/sdm  10/20/09 First release
* </pre>
*
******************************************************************************/
#ifndef XREG_CORTEXA9_H
#define XREG_CORTEXA9_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* GPRs */
#define XREG_GPR0				r0
#define XREG_GPR1				r1
#define XREG_GPR2				r2
#define XREG_GPR3				r3
#define XREG_GPR4				r4
#define XREG_GPR5				r5
#define XREG_GPR6				r6
#define XREG_GPR7				r7
#define XREG_GPR8				r8
#define XREG_GPR9				r9
#define XREG_GPR10				r10
#define XREG_GPR11				r11
#define XREG_GPR12				r12
#define XREG_GPR13				r13
#define XREG_GPR14				r14
#define XREG_GPR15				r15
#define XREG_CPSR				cpsr

/* Coprocessor number defines */
#define XREG_CP0				0
#define XREG_CP1				1
#define XREG_CP2				2
#define XREG_CP3				3
#define XREG_CP4				4
#define XREG_CP5				5
#define XREG_CP6				6
#define XREG_CP7				7
#define XREG_CP8				8
#define XREG_CP9				9
#define XREG_CP10				10
#define XREG_CP11				11
#define XREG_CP12				12
#define XREG_CP13				13
#define XREG_CP14				14
#define XREG_CP15				15

/* Coprocessor control register defines */
#define XREG_CR0				cr0
#define XREG_CR1				cr1
#define XREG_CR2				cr2
#define XREG_CR3				cr3
#define XREG_CR4				cr4
#define XREG_CR5				cr5
#define XREG_CR6				cr6
#define XREG_CR7				cr7
#define XREG_CR8				cr8
#define XREG_CR9				cr9
#define XREG_CR10				cr10
#define XREG_CR11				cr11
#define XREG_CR12				cr12
#define XREG_CR13				cr13
#define XREG_CR14				cr14
#define XREG_CR15				cr15

/* Current Processor Status Register (CPSR) Bits */
#define XREG_CPSR_THUMB_MODE			0x20
#define XREG_CPSR_MODE_BITS			0x1F
#define XREG_CPSR_SYSTEM_MODE			0x1F
#define XREG_CPSR_UNDEFINED_MODE		0x1B
#define XREG_CPSR_DATA_ABORT_MODE		0x17
#define XREG_CPSR_SVC_MODE			0x13
#define XREG_CPSR_IRQ_MODE			0x12
#define XREG_CPSR_FIQ_MODE			0x11
#define XREG_CPSR_USER_MODE			0x10

#define XREG_CPSR_IRQ_ENABLE			0x80
#define XREG_CPSR_FIQ_ENABLE			0x40

#define XREG_CPSR_N_BIT				0x80000000
#define XREG_CPSR_Z_BIT				0x40000000
#define XREG_CPSR_C_BIT				0x20000000
#define XREG_CPSR_V_BIT				0x10000000


/* CP15 defines */

/* C0 Register defines */
#define XREG_CP15_MAIN_ID			"p15, 0, %0,  c0,  c0, 0"
#define XREG_CP15_CACHE_TYPE			"p15, 0, %0,  c0,  c0, 1"
#define XREG_CP15_TCM_TYPE			"p15, 0, %0,  c0,  c0, 2"
#define XREG_CP15_TLB_TYPE			"p15, 0, %0,  c0,  c0, 3"
#define XREG_CP15_MULTI_PROC_AFFINITY		"p15, 0, %0,  c0,  c0, 5"

#define XREG_CP15_PROC_FEATURE_0		"p15, 0, %0,  c0,  c1, 0"
#define XREG_CP15_PROC_FEATURE_1		"p15, 0, %0,  c0,  c1, 1"
#define XREG_CP15_DEBUG_FEATURE_0		"p15, 0, %0,  c0,  c1, 2"
#define XREG_CP15_MEMORY_FEATURE_0		"p15, 0, %0,  c0,  c1, 4"
#define XREG_CP15_MEMORY_FEATURE_1		"p15, 0, %0,  c0,  c1, 5"
#define XREG_CP15_MEMORY_FEATURE_2		"p15, 0, %0,  c0,  c1, 6"
#define XREG_CP15_MEMORY_FEATURE_3		"p15, 0, %0,  c0,  c1, 7"

#define XREG_CP15_INST_FEATURE_0		"p15, 0, %0,  c0,  c2, 0"
#define XREG_CP15_INST_FEATURE_1		"p15, 0, %0,  c0,  c2, 1"
#define XREG_CP15_INST_FEATURE_2		"p15, 0, %0,  c0,  c2, 2"
#define XREG_CP15_INST_FEATURE_3		"p15, 0, %0,  c0,  c2, 3"
#define XREG_CP15_INST_FEATURE_4		"p15, 0, %0,  c0,  c2, 4"

#define XREG_CP15_CACHE_SIZE_ID			"p15, 1, %0,  c0,  c0, 0"
#define XREG_CP15_CACHE_LEVEL_ID		"p15, 1, %0,  c0,  c0, 1"
#define XREG_CP15_AUXILARY_ID			"p15, 1, %0,  c0,  c0, 7"

#define XREG_CP15_CACHE_SIZE_SEL		"p15, 2, %0,  c0,  c0, 0"

/* C1 Register Defines */
#define XREG_CP15_SYS_CONTROL			"p15, 0, %0,  c1,  c0, 0"
#define XREG_CP15_AUX_CONTROL			"p15, 0, %0,  c1,  c0, 1"
#define XREG_CP15_CP_ACCESS_CONTROL		"p15, 0, %0,  c1,  c0, 2"

#define XREG_CP15_SECURE_CONFIG			"p15, 0, %0,  c1,  c1, 0"
#define XREG_CP15_SECURE_DEBUG_ENABLE		"p15, 0, %0,  c1,  c1, 1"
#define XREG_CP15_NS_ACCESS_CONTROL		"p15, 0, %0,  c1,  c1, 2"
#define XREG_CP15_VIRTUAL_CONTROL		"p15, 0, %0,  c1,  c1, 3"

/* XREG_CP15_CONTROL bit defines */
#define XREG_CP15_CONTROL_TE_BIT		0x40000000
#define XREG_CP15_CONTROL_AFE_BIT		0x20000000
#define XREG_CP15_CONTROL_TRE_BIT		0x10000000
#define XREG_CP15_CONTROL_NMFI_BIT		0x08000000
#define XREG_CP15_CONTROL_EE_BIT		0x02000000
#define XREG_CP15_CONTROL_HA_BIT		0x00020000
#define XREG_CP15_CONTROL_RR_BIT		0x00004000
#define XREG_CP15_CONTROL_V_BIT			0x00002000
#define XREG_CP15_CONTROL_I_BIT			0x00001000
#define XREG_CP15_CONTROL_Z_BIT			0x00000800
#define XREG_CP15_CONTROL_SW_BIT		0x00000400
#define XREG_CP15_CONTROL_B_BIT			0x00000080
#define XREG_CP15_CONTROL_C_BIT			0x00000004
#define XREG_CP15_CONTROL_A_BIT			0x00000002
#define XREG_CP15_CONTROL_M_BIT			0x00000001

/* C2 Register Defines */
#define XREG_CP15_TTBR0				"p15, 0, %0,  c2,  c0, 0"
#define XREG_CP15_TTBR1				"p15, 0, %0,  c2,  c0, 1"
#define XREG_CP15_TTB_CONTROL			"p15, 0, %0,  c2,  c0, 2"

/* C3 Register Defines */
#define XREG_CP15_DOMAIN_ACCESS_CTRL		"p15, 0, %0,  c3,  c0, 0"

/* C4 Register Defines */
/* Not Used */

/* C5 Register Defines */
#define XREG_CP15_DATA_FAULT_STATUS		"p15, 0, %0,  c5,  c0, 0"
#define XREG_CP15_INST_FAULT_STATUS		"p15, 0, %0,  c5,  c0, 1"

#define XREG_CP15_AUX_DATA_FAULT_STATUS		"p15, 0, %0,  c5,  c1, 0"
#define XREG_CP15_AUX_INST_FAULT_STATUS		"p15, 0, %0,  c5,  c1, 1"

/* C6 Register Defines */
#define XREG_CP15_DATA_FAULT_ADDRESS		"p15, 0, %0,  c6,  c0, 0"
#define XREG_CP15_INST_FAULT_ADDRESS		"p15, 0, %0,  c6,  c0, 2"

/* C7 Register Defines */
#define XREG_CP15_NOP				"p15, 0, %0,  c7,  c0, 4"

#define XREG_CP15_INVAL_IC_POU_IS		"p15, 0, %0,  c7,  c1, 0"
#define XREG_CP15_INVAL_BRANCH_ARRAY_IS		"p15, 0, %0,  c7,  c1, 6"

#define XREG_CP15_PHYS_ADDR			"p15, 0, %0,  c7,  c4, 0"

#define XREG_CP15_INVAL_IC_POU			"p15, 0, %0,  c7,  c5, 0"
#define XREG_CP15_INVAL_IC_LINE_MVA_POU		"p15, 0, %0,  c7,  c5, 1"
#define XREG_CP15_INST_SYNC_BARRIER		"p15, 0, %0,  c7,  c5, 4"
#define XREG_CP15_INVAL_BRANCH_ARRAY		"p15, 0, %0,  c7,  c5, 6"

#define XREG_CP15_INVAL_DC_LINE_MVA_POC		"p15, 0, %0,  c7,  c6, 1"
#define XREG_CP15_INVAL_DC_LINE_SW		"p15, 0, %0,  c7,  c6, 2"

#define XREG_CP15_VA_TO_PA_CURRENT_0		"p15, 0, %0,  c7,  c8, 0"
#define XREG_CP15_VA_TO_PA_CURRENT_1		"p15, 0, %0,  c7,  c8, 1"
#define XREG_CP15_VA_TO_PA_CURRENT_2		"p15, 0, %0,  c7,  c8, 2"
#define XREG_CP15_VA_TO_PA_CURRENT_3		"p15, 0, %0,  c7,  c8, 3"

#define XREG_CP15_VA_TO_PA_OTHER_0		"p15, 0, %0,  c7,  c8, 4"
#define XREG_CP15_VA_TO_PA_OTHER_1		"p15, 0, %0,  c7,  c8, 5"
#define XREG_CP15_VA_TO_PA_OTHER_2		"p15, 0, %0,  c7,  c8, 6"
#define XREG_CP15_VA_TO_PA_OTHER_3		"p15, 0, %0,  c7,  c8, 7"

#define XREG_CP15_CLEAN_DC_LINE_MVA_POC		"p15, 0, %0,  c7,  c10, 1"
#define XREG_CP15_CLEAN_DC_LINE_SW		"p15, 0, %0,  c7,  c10, 2"
#define XREG_CP15_DATA_SYNC_BARRIER		"p15, 0, %0,  c7,  c10, 4"
#define XREG_CP15_DATA_MEMORY_BARRIER		"p15, 0, %0,  c7,  c10, 5"

#define XREG_CP15_CLEAN_DC_LINE_MVA_POU		"p15, 0, %0,  c7,  c11, 1"

#define XREG_CP15_NOP2				"p15, 0, %0,  c7,  c13, 1"

#define XREG_CP15_CLEAN_INVAL_DC_LINE_MVA_POC	"p15, 0, %0,  c7,  c14, 1"
#define XREG_CP15_CLEAN_INVAL_DC_LINE_SW	"p15, 0, %0,  c7,  c14, 2"

/* C8 Register Defines */
#define XREG_CP15_INVAL_TLB_IS			"p15, 0, %0,  c8,  c3, 0"
#define XREG_CP15_INVAL_TLB_MVA_IS		"p15, 0, %0,  c8,  c3, 1"
#define XREG_CP15_INVAL_TLB_ASID_IS		"p15, 0, %0,  c8,  c3, 2"
#define XREG_CP15_INVAL_TLB_MVA_ASID_IS		"p15, 0, %0,  c8,  c3, 3"

#define XREG_CP15_INVAL_ITLB_UNLOCKED		"p15, 0, %0,  c8,  c5, 0"
#define XREG_CP15_INVAL_ITLB_MVA		"p15, 0, %0,  c8,  c5, 1"
#define XREG_CP15_INVAL_ITLB_ASID		"p15, 0, %0,  c8,  c5, 2"

#define XREG_CP15_INVAL_DTLB_UNLOCKED		"p15, 0, %0,  c8,  c6, 0"
#define XREG_CP15_INVAL_DTLB_MVA		"p15, 0, %0,  c8,  c6, 1"
#define XREG_CP15_INVAL_DTLB_ASID		"p15, 0, %0,  c8,  c6, 2"

#define XREG_CP15_INVAL_UTLB_UNLOCKED		"p15, 0, %0,  c8,  c7, 0"
#define XREG_CP15_INVAL_UTLB_MVA		"p15, 0, %0,  c8,  c7, 1"
#define XREG_CP15_INVAL_UTLB_ASID		"p15, 0, %0,  c8,  c7, 2"
#define XREG_CP15_INVAL_UTLB_MVA_ASID		"p15, 0, %0,  c8,  c7, 3"

/* C9 Register Defines */
#define XREG_CP15_PERF_MONITOR_CTRL		"p15, 0, %0,  c9,  c12, 0"
#define XREG_CP15_COUNT_ENABLE_SET		"p15, 0, %0,  c9,  c12, 1"
#define XREG_CP15_COUNT_ENABLE_CLR		"p15, 0, %0,  c9,  c12, 2"
#define XREG_CP15_V_FLAG_STATUS			"p15, 0, %0,  c9,  c12, 3"
#define XREG_CP15_SW_INC			"p15, 0, %0,  c9,  c12, 4"
#define XREG_CP15_EVENT_CNTR_SEL		"p15, 0, %0,  c9,  c12, 5"

#define XREG_CP15_PERF_CYCLE_COUNTER		"p15, 0, %0,  c9,  c13, 0"
#define XREG_CP15_EVENT_TYPE_SEL		"p15, 0, %0,  c9,  c13, 1"
#define XREG_CP15_PERF_MONITOR_COUNT		"p15, 0, %0,  c9,  c13, 2"

#define XREG_CP15_USER_ENABLE			"p15, 0, %0,  c9,  c14, 0"
#define XREG_CP15_INTR_ENABLE_SET		"p15, 0, %0,  c9,  c14, 1"
#define XREG_CP15_INTR_ENABLE_CLR		"p15, 0, %0,  c9,  c14, 2"

/* C10 Register Defines */
#define XREG_CP15_TLB_LOCKDWN			"p15, 0, %0,  c10,  c0, 0"

#define XREG_CP15_PRI_MEM_REMAP			"p15, 0, %0,  c10,  c2, 0"
#define XREG_CP15_NORM_MEM_REMAP		"p15, 0, %0,  c10,  c2, 1"

/* C11 Register Defines */
/* Not used */

/* C12 Register Defines */
#define XREG_CP15_VEC_BASE_ADDR			"p15, 0, %0,  c12,  c0, 0"
#define XREG_CP15_MONITOR_VEC_BASE_ADDR		"p15, 0, %0,  c12,  c0, 1"

#define XREG_CP15_INTERRUPT_STATUS		"p15, 0, %0,  c12,  c1, 0"
#define XREG_CP15_VIRTUALIZATION_INTR		"p15, 0, %0,  c12,  c1, 1"

/* C13 Register Defines */
#define XREG_CP15_CONTEXT_ID			"p15, 0, %0,  c13,  c0, 1"
#define USER_RW_THREAD_PID			"p15, 0, %0,  c13,  c0, 2"
#define USER_RO_THREAD_PID			"p15, 0, %0,  c13,  c0, 3"
#define USER_PRIV_THREAD_PID			"p15, 0, %0,  c13,  c0, 4"

/* C14 Register Defines */
/* not used */

/* C15 Register Defines */
#define XREG_CP15_POWER_CTRL			"p15, 0, %0,  c15,  c0, 0"
#define XREG_CP15_CONFIG_BASE_ADDR		"p15, 4, %0,  c15,  c0, 0"

#define XREG_CP15_READ_TLB_ENTRY		"p15, 5, %0,  c15,  c4, 2"
#define XREG_CP15_WRITE_TLB_ENTRY		"p15, 5, %0,  c15,  c4, 4"

#define XREG_CP15_MAIN_TLB_VA			"p15, 5, %0,  c15,  c5, 2"

#define XREG_CP15_MAIN_TLB_PA			"p15, 5, %0,  c15,  c6, 2"

#define XREG_CP15_MAIN_TLB_ATTR			"p15, 5, %0,  c15,  c7, 2"

/* VFP register definitions. First, the standard VFP set. */
#define XREG_FPSID				cr0
#define XREG_FPSCR				cr1
#define XREG_MVFR1				cr6
#define XREG_MVFR0				cr7
#define XREG_FPEXC				cr8
#define XREG_FPINST				cr9
#define XREG_FPINST2				cr10

/* FPSID bits */
#define XREG_FPSID_IMPLEMENTER_BIT	(24)
#define XREG_FPSID_IMPLEMENTER_MASK	(0xff << FPSID_IMPLEMENTER_BIT)
#define XREG_FPSID_SOFTWARE		(1<<23)
#define XREG_FPSID_FORMAT_BIT		(21)
#define XREG_FPSID_FORMAT_MASK		(0x3  << FPSID_FORMAT_BIT)
#define XREG_FPSID_NODOUBLE		(1 << 20)
#define XREG_FPSID_ARCH_BIT		(16)
#define XREG_FPSID_ARCH_MASK		(0xF  << FPSID_ARCH_BIT)
#define XREG_FPSID_PART_BIT		(8)
#define XREG_FPSID_PART_MASK		(0xFF << FPSID_PART_BIT)
#define XREG_FPSID_VARIANT_BIT		(4)
#define XREG_FPSID_VARIANT_MASK		(0xF  << FPSID_VARIANT_BIT)
#define XREG_FPSID_REV_BIT		(0)
#define XREG_FPSID_REV_MASK		(0xF  << FPSID_REV_BIT)

/* FPSCR bits */
#define XREG_FPSCR_QC			(1 << 27)
#define XREG_FPSCR_AHP			(1 << 26)
#define XREG_FPSCR_DEFAULT_NAN		(1 << 25)
#define XREG_FPSCR_FLUSHTOZERO		(1 << 24)
#define XREG_FPSCR_ROUND_NEAREST	(0 << 22)
#define XREG_FPSCR_ROUND_PLUSINF	(1 << 22)
#define XREG_FPSCR_ROUND_MINUSINF	(2 << 22)
#define XREG_FPSCR_ROUND_TOZERO		(3 << 22)
#define XREG_FPSCR_RMODE_BIT		(22)
#define XREG_FPSCR_RMODE_MASK		(3 << FPSCR_RMODE_BIT)
#define XREG_FPSCR_STRIDE_BIT		(20)
#define XREG_FPSCR_STRIDE_MASK		(3 << FPSCR_STRIDE_BIT)
#define XREG_FPSCR_LENGTH_BIT		(16)
#define XREG_FPSCR_LENGTH_MASK		(7 << FPSCR_LENGTH_BIT)
#define XREG_FPSCR_IDC			(1 << 7)
#define XREG_FPSCR_IXC			(1 << 4)
#define XREG_FPSCR_UFC			(1 << 3)
#define XREG_FPSCR_OFC			(1 << 2)
#define XREG_FPSCR_DZC			(1 << 1)
#define XREG_FPSCR_IOC			(1 << 0)

/* MVFR0 bits */
#define XREG_MVFR0_A_SIMD_BIT		(0)
#define XREG_MVFR0_A_SIMD_MASK		(0xf << MVFR0_A_SIMD_BIT)

/* FPEXC bits */
#define XREG_FPEXC_EX			(1 << 31)
#define XREG_FPEXC_EN			(1 << 30)
#define XREG_FPEXC_DEX			(1 << 29)
#define XREG_FPEXC_FP2V			(1 << 28)
#define XREG_FPEXC_VV			(1 << 27)
#define XREG_FPEXC_TFV			(1 << 26)
#define XREG_FPEXC_LENGTH_BIT		(8)
#define XREG_FPEXC_LENGTH_MASK		(7 << FPEXC_LENGTH_BIT)
#define XREG_FPEXC_IDF			(1 << 7)
#define XREG_FPEXC_IXF			(1 << 4)
#define XREG_FPEXC_UFF			(1 << 3)
#define XREG_FPEXC_OFF			(1 << 2)
#define XREG_FPEXC_DZF			(1 << 1)
#define XREG_FPEXC_IOF			(1 << 0)
#define XREG_FPEXC_TRAP_MASK		(FPEXC_IDF | FPEXC_IXF | FPEXC_UFF | \
					 FPEXC_OFF | FPEXC_DZF | FPEXC_IOF)

/* Bit patterns for decoding the packaged operation descriptors */
#define XREG_VFPOPDESC_LENGTH_BIT	(9)
#define XREG_VFPOPDESC_LENGTH_MASK	(0x07 << VFPOPDESC_LENGTH_BIT)
#define XREG_VFPOPDESC_UNUSED_BIT	(24)
#define XREG_VFPOPDESC_UNUSED_MASK	(0xFF << VFPOPDESC_UNUSED_BIT)
#define XREG_VFPOPDESC_OPDESC_MASK	(~(VFPOPDESC_LENGTH_MASK | \
					   VFPOPDESC_UNUSED_MASK))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* XREG_CORTEXA9_H */
