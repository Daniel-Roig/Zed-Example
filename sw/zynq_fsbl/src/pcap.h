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
* @file pcap.h
*
* This file contains the interface for intiializing and accessing the PCAP
*
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date        Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00a ecm  02/10/10 Initial release
*
* </pre>
*
* @note
*
******************************************************************************/
#ifndef ___PCAP_H___
#define ___PCAP_H___


#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "top_level.h"
#include "xparameters_pss.h"

#define WTO_DT595848_WORKAROUND

/************************** Constant Definitions *****************************/

#define PCAP_CONTROL_REG      (0x000)   /**< Control Register */
#define PCAP_LOCK_REG         (0x004)   /**< Lock Register */
#define PCAP_CONFIG_REG       (0x008)   /**< Configuration Register */
#define PCAP_ISR              (0x00C)   /**< Interrupt Status Register */
#define PCAP_IMR              (0x010)   /**< Interrupt Mask Register */
#define PCAP_STATUS_REG       (0x014)   /**< Status Register */
#define PCAP_DMA_SRC_REG      (0x018)   /**< DMA Source Address Register */
#define PCAP_DMA_DEST_REG     (0x01C)   /**< DMA Destination Address Register */
#define PCAP_DMA_LEN_REG      (0x020)   /**< DMA src Length Register */
#define PCAP_DMA_DEST_LEN_REG (0x024)   /**< DMA dst Length Register */
#define PCAP_ROM_SHADOW_CRTL  (0x028)   /**< ROM Shadow Control Register */
#define PCAP_MBOOT_REG        (0x02C)   /**< Multi-Boot Offset Register */
#define PCAP_SW_ID_REG        (0x030)   /**< Software ID Register */
#define PCAP_UNLOCK_REG       (0x034)   /**< Unlock Register */
#define PCAP_MCTRL_REG        (0x080)   /**< Miscellaneous Control Register */

/* Si Debug Registers */
#define PCAP_DEBUG_XFER_WRITE_COUNT_OFFSET (0x088) /**< write transfer count*/
#define PCAP_DEBUG_XFER_READ_COUNT_OFFSET  (0x08C) /**< read transfer count */

/* Bit postiions for PCAP_CONTROL_REG */
#define PCAP_CONTROL_RESET_MASK  (0x80000000) /**< Force PSS secure reset */
#define PCAP_CONTROL_PROG_B_MASK (0x40000000) /**< Reset FPGA */
#define PCAP_CONTROL_PR_MASK     (0x08000000) /**< Internal Partial Re-Config*/
#define PCAP_CONTROL_PCAP_MODE_MASK (0x04000000) /**< Disable FPGA external */
#define PCAP_CONTROL_QTR_PCAP_MASK  (0x02000000) /**< Quarter rate */
#define PCAP_CONTROL_MULTIBOOT_MASK (0x01000000) /**< Enables POR multi-boot */
#define PCAP_CONTROL_DIS_JTAG_MASK  (0x00800000) /**< Disable JTAG to FPGA */
#define PCAP_CONTROL_USER_MODE_MASK (0x00008000) /**< CPU in USER mode */
#define PCAP_CONTROL_PCFG_AES_FUSE_MASK (0x00001000) /**< AES key E-Fuse */
#define PCAP_CONTROL_PCFG_AES_MASK  (0x00000E00) /**< Enable AES engine */
#define PCAP_CONTROL_SEU_MASK       (0x00000100) /**< Reset PSS when SEU */
#define PCAP_CONTROL_NIDEN_MASK     (0x00000010) /**< Non-Invasive Debug */
#define PCAP_CONTROL_DBGEN_MASK     (0x00000008) /**< Invasive Debug Enable */
#define PCAP_CONTROL_DAP_MASK       (0x00000007) /**< Enable ARM DAP */

/**< Bit Defines for PCAP_LOCK_REG */
#define PCAP_LOCK_AES_MASK (0x00000080) /**< AES_EN, POR recover */

#define PCAP_LOCK_SEU_MASK (0x00000040) /**< SEU_En, POR recover */

#define PCAP_LOCK_SEC_MASK (0x00000002) /**< SEC_EN/USER_MODE, POR recover */

#define PCAP_LOCK_DBG_MASK (0x00000001) /**< DAP/DBB/NID/SPNI_EN, POR recover*/ 

/**< Bit Defines for PCAP_CONFIG_REG */
#define PCAP_CONFIG_RFIFO_TH_MASK  (0x00000C00) /**< Rx threshold for Intr */
#define PCAP_CONFIG_WFIFO_TH_MASK  (0x00000300) /**< Tx threshold for Intr */
#define PCAP_CONFIG_RCLK_EDGE_MASK (0x00000080) /**< Read active clock edge */
#define PCAP_CONFIG_WCLK_EDGE_MASK (0x00000040) /**< Write active clock edge */
#define PCAP_CONFIG_RD_ISSU_MASK   (0x00000008) /**< Read issue capability */
#define PCAP_CONFIG_WR_ISSU_MASK   (0x00000004) /**< Write issue capability */
#define PCAP_CONFIG_RDLEN_MASK     (0x00000002) /**< Read burst length */
#define PCAP_CONFIG_WRLEN_MASK     (0x00000001) /**< Write burst length */

/* Bit Defines for PCAP_ISR and PCAP_IMR */
#define PCAP_IXR_AXI_WTO_MASK      (0x00800000) /**< Write time out */
#define PCAP_IXR_AXI_WERR_MASK     (0x00400000) /**< Write response error */
#define PCAP_IXR_AXI_RTO_MASK      (0x00200000) /**< Read time out */
#define PCAP_IXR_AXI_RERR_MASK     (0x00100000) /**< Read response error */
#define PCAP_IXR_RX_FIFO_OV_MASK   (0x00040000) /**< RX FIFO overflows */
#define PCAP_IXR_WR_FIFO_LVL_INT_MASK (0x00020000) /**< Tx FIFO <= threshold */
#define PCAP_IXR_RD_FIFO_LVL_INT_MASK (0x00010000) /**< Rx FIFO >= threshold */
#define PCAP_IXR_DMA_CMD_ERR_MASK  (0x00008000) /**< Illegal DMA command */
#define PCAP_IXR_DMA_Q_OV_MASK     (0x00004000) /**< DMA cmd queue overflow*/
#define PCAP_IXR_DMA_DONE_MASK     (0x00002000) /**< DMA transfer done */
#define PCAP_IXR_PCAP_DMA_DONE_MASK   (0x00001000) /**< PCAP transfer done */
#define PCAP_IXR_P2D_LEN_ERR_INT      (0x00000800) /**< Length Mismatch */
#define PCAP_IXR_PCFG_HMAC_ERR_MASK   (0x00000040) /**< HMAC error from FPGA */
#define PCAP_IXR_PCFG_SEU_ERR_MASK    (0x00000020) /**< SEU error from FPGA */
#define PCAP_IXR_PCFG_POR_B_MASK      (0x00000010) /**< FPGA POR Indication */
#define PCAP_IXR_PCFG_CFG_RST_MASK    (0x00000008) /**< FPGA under reset */
#define PCAP_IXR_PCFG_DONE_MASK       (0x00000004) /**< FPGA program done */
#define PCAP_IXR_PCFG_PE_INIT_MASK    (0x00000002) /**< +ve init from FPGA */
#define PCAP_IXR_PCFG_NE_INIT_MASK    (0x00000001) /**< -ve init from FPGA */

/* FATAL errors, FSBL fallback */
#ifndef WTO_DT595848_WORKAROUND
#define PCAP_IXR_FATAL_ERROR_MASK          (PCAP_IXR_PCFG_HMAC_ERR_MASK | \
                                            PCAP_IXR_AXI_WTO_MASK |       \
                                            PCAP_IXR_AXI_WERR_MASK |      \
                                            PCAP_IXR_AXI_RTO_MASK |       \
                                            PCAP_IXR_AXI_RERR_MASK |      \
                                            PCAP_IXR_RX_FIFO_OV_MASK)

#define ALL_PCAP_IXR_ERROR_FLAGS_MASK      (PCAP_IXR_AXI_WTO_MASK |       \
                                            PCAP_IXR_AXI_WERR_MASK |      \
                                            PCAP_IXR_AXI_RTO_MASK |       \
                                            PCAP_IXR_AXI_RERR_MASK |      \
                                            PCAP_IXR_RX_FIFO_OV_MASK |    \
                                            PCAP_IXR_DMA_CMD_ERR_MASK |   \
                                            PCAP_IXR_DMA_Q_OV_MASK |      \
                                            PCAP_IXR_P2D_LEN_ERR_INT |    \
                                            PCAP_IXR_PCFG_HMAC_ERR_MASK )

#define PCAP_IXR_ERROR_FLAGS_MASK           (PCAP_IXR_AXI_WTO_MASK |      \
                                             PCAP_IXR_AXI_RTO_MASK |      \
                                             PCAP_IXR_AXI_RERR_MASK |     \
                                             PCAP_IXR_RX_FIFO_OV_MASK |   \
                                             PCAP_IXR_P2D_LEN_ERR_INT |   \
                                             PCAP_IXR_PCFG_HMAC_ERR_MASK )
#else
#define PCAP_IXR_FATAL_ERROR_MASK          (PCAP_IXR_PCFG_HMAC_ERR_MASK | \
                                            PCAP_IXR_AXI_WERR_MASK |      \
                                            PCAP_IXR_AXI_RTO_MASK |       \
                                            PCAP_IXR_AXI_RERR_MASK |      \
                                            PCAP_IXR_RX_FIFO_OV_MASK)

#define ALL_PCAP_IXR_ERROR_FLAGS_MASK      (PCAP_IXR_AXI_WERR_MASK |      \
                                            PCAP_IXR_AXI_RTO_MASK |       \
                                            PCAP_IXR_AXI_RERR_MASK |      \
                                            PCAP_IXR_RX_FIFO_OV_MASK |    \
                                            PCAP_IXR_DMA_CMD_ERR_MASK |   \
                                            PCAP_IXR_DMA_Q_OV_MASK |      \
                                            PCAP_IXR_P2D_LEN_ERR_INT |    \
                                            PCAP_IXR_PCFG_HMAC_ERR_MASK )

#define PCAP_IXR_ERROR_FLAGS_MASK      (PCAP_IXR_AXI_RTO_MASK |         \
                                        PCAP_IXR_AXI_RERR_MASK |        \
                                        PCAP_IXR_RX_FIFO_OV_MASK |      \
                                        PCAP_IXR_P2D_LEN_ERR_INT |      \
                                        PCAP_IXR_PCFG_HMAC_ERR_MASK )
#endif  /* WTO_DT595848_WORKAROUND */

/* Bit Mask Defines for PCAP_STATUS_REG */
#define PCAP_STATUS_DMA_CMD_Q_F_MASK   (0x80000000) /**< DMA cmd queue full */
#define PCAP_STATUS_DMA_CMD_Q_E_MASK   (0x40000000) /**< DMA cmd queue empty */
#define PCAP_STATUS_DMA_DONE_CNT_MASK  (0x30000000) /**< DMA done count */
#define PCAP_STATUS_RX_FIFO_LVL_MASK   (0x01F80000) /**< Words in Rx FIFO */
#define PCAP_STATUS_TXFIFO_LVL_MASK    (0x0007F800) /**< Words in Tx FIFO */
#define PCAP_STATUS_SECURE_RST_MASK    (0x00000080) /**< Secure violation rst */
#define PCAP_STATUS_ILLEGAL_APB_ACCESS (0x00000040) /**< Illegal APB access */
#define PCAP_STATUS_PCFG_INIT_MASK     (0x00000010) /**< Fabric power status */
#define PCAP_STATUS_EFUSE_SW_RSRV_MASK (0x00000008) /**< Efuse reserve bit */
#define PCAP_STATUS_EFUSE_SEC_EN_MASK  (0x00000004) /**< Efuse security enable*/
#define PCAP_STATUS_EFUSE_JTAG_DIS_MASK (0x00000002) /**<Efuse jtag disable */
#define PCAP_STATUS_SECURE_HACK_MASK    (0x00000001) /**< Had security breach */

/* Bit Defines for PCAP_MCTRL_REG */
#define PCAP_MCTRL_DMAQ_LOOPBACK_MASK (0x00000010) /**< DMA loopback */
#define PCAP_MCTRL_RFIFO_FLUSH_MASK   (0x00000002) /**< Flush Rx FIFO */
#define PCAP_MCTRL_WFIFO_FLUSH_MASK   (0x00000001) /**< Flush Tx FIFO */

/* DMA source and destination invalid value. */
#define PCAP_DMA_INVALID_ADDRESS      (0xFFFFFFFF)

/* PCAP and AXI together have a 64-byte alignment requirement for crossing
 * 1K boundary */
#define PCAP_64_BYTE_ALIGN_MASK                0x3F
#define PCAP_ONE_K_MASK                        0x3FF
#define PCAP_ONE_K_SIZE                        0x400

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/
#define PcapIn32        Xil_In32
#define PcapOut32       Xil_Out32

/************************** Function Prototypes ******************************/
int InitPcap(u32 Data);

u32 WritePcapXferData(u32 *SourceData, u32 *DestinationData,
            u32 SourceLength,u32 DestinationLength, u32 Flags);

/************************** Variable Definitions *****************************/
#ifdef __cplusplus
}
#endif


#endif /* ___PCAP_H___ */

