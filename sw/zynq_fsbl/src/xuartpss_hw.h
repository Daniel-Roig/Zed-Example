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
* @file xuartpss_hw.h
*
* This header file contains the hardware interface of an XUartPss device.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who     Date     Changes
* ----- ------ -------- ----------------------------------------------
* 1.00	drg/jz 05/12/08 First Release
* </pre>
*
******************************************************************************/

#ifndef XUARTPSS_HW_H		/* prevent circular inclusions */
#define XUARTPSS_HW_H		/* by using protection macros */

#ifdef __cplusplus
extern "C" {
#endif

/***************************** Include Files *********************************/

#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"

/************************** Constant Definitions *****************************/

/** @name Register Map
 *
 * Register offsets for the UART.
 * @{
 */
#define XUARTPSS_CR_OFFSET          0x00  /**< Control Register [8:0] */
#define XUARTPSS_MR_OFFSET          0x04  /**< Mode Register [10:0] */
#define XUARTPSS_IER_OFFSET         0x08  /**< Interrupt Enable [10:0] */
#define XUARTPSS_IDR_OFFSET         0x0C  /**< Interrupt Disable [10:0] */
#define XUARTPSS_IMR_OFFSET         0x10  /**< Interrupt Mask [10:0] */
#define XUARTPSS_ISR_OFFSET         0x14  /**< Interrupt Status [10:0]*/
#define XUARTPSS_BAUDGEN_OFFSET     0x18  /**< Baud Rate Generator [15:0] */
#define XUARTPSS_RXTOUT_OFFSET      0x1C  /**< RX Timeout [7:0] */
#define XUARTPSS_RXWM_OFFSET        0x20  /**< RX FIFO Trigger Level [5:0] */
#define XUARTPSS_MODEMCR_OFFSET     0x24  /**< Modem Control [5:0] */
#define XUARTPSS_MODEMSR_OFFSET     0x28  /**< Modem Status [8:0] */
#define XUARTPSS_SR_OFFSET          0x2C  /**< Channel Status [11:0] */
#define XUARTPSS_FIFO_OFFSET        0x30  /**< FIFO [15:0] or [7:0] */
#define XUARTPSS_BAUDDIV_OFFSET     0x34  /**< Baud Rate Divider [7:0] */
#define XUARTPSS_FLOWDEL_OFFSET     0x38  /**< Flow Delay [15:0] */
/* @} */

/** @name Control Register
 *
 * The Control register (CR) controls the major functions of the device.
 *
 * Control Register Bit Definition
 */

#define XUARTPSS_CR_STOPBRK     0x00000100  /**< Stop transmission of break */
#define XUARTPSS_CR_STARTBRK    0x00000080  /**< Set break */
#define XUARTPSS_CR_TORST       0x00000040  /**< RX timeout counter restart */
#define XUARTPSS_CR_TX_DIS      0x00000020  /**< TX disabled. */
#define XUARTPSS_CR_TX_EN       0x00000010  /**< TX enabled */
#define XUARTPSS_CR_RX_DIS      0x00000008  /**< RX disabled. */
#define XUARTPSS_CR_RX_EN       0x00000004  /**< RX enabled */
#define XUARTPSS_CR_EN_DIS_MASK 0x0000003C  /**< Enable/disable Mask */
#define XUARTPSS_CR_TXRST       0x00000002  /**< TX logic reset */
#define XUARTPSS_CR_RXRST       0x00000001  /**< RX logic reset */
/* @}*/


/** @name Mode Register
 *
 * The mode register (MR) defines the mode of transfer as well as the data
 * format. If this register is modified during transmission or reception,
 * data validity cannot be guaranteed.
 *
 * Mode Register Bit Definition
 * @{
 */

#define XUARTPSS_MR_CCLK             0x00000400  /**< Input clock selection */
#define XUARTPSS_MR_CHMODE_R_LOOP    0x00000300  /**< Remote loopback mode */
#define XUARTPSS_MR_CHMODE_L_LOOP    0x00000200  /**< Local loopback mode */
#define XUARTPSS_MR_CHMODE_ECHO      0x00000100  /**< Auto echo mode */
#define XUARTPSS_MR_CHMODE_NORM      0x00000000  /**< Normal mode */
#define XUARTPSS_MR_CHMODE_SHIFT     8	   /**< Mode shift */
#define XUARTPSS_MR_CHMODE_MASK      0x00000300  /**< Mode mask */
#define XUARTPSS_MR_STOPMODE_2_BIT   0x00000080  /**< 2 stop bits */
#define XUARTPSS_MR_STOPMODE_1_5_BIT 0x00000040  /**< 1.5 stop bits */
#define XUARTPSS_MR_STOPMODE_1_BIT   0x00000000  /**< 1 stop bit */
#define XUARTPSS_MR_STOPMODE_SHIFT   6	   /**< Stop bits setting shift */
#define XUARTPSS_MR_STOPMODE_MASK    0x000000A0  /**< Stop bits setting mask */
#define XUARTPSS_MR_PARITY_NONE      0x00000020  /**< No parity mode */
#define XUARTPSS_MR_PARITY_MARK      0x00000018  /**< Mark parity mode */
#define XUARTPSS_MR_PARITY_SPACE     0x00000010  /**< Space parity mode */
#define XUARTPSS_MR_PARITY_ODD       0x00000008  /**< Odd parity mode */
#define XUARTPSS_MR_PARITY_EVEN      0x00000000  /**< Even parity mode */
#define XUARTPSS_MR_PARITY_SHIFT     3	     /**< Parity setting shift */
#define XUARTPSS_MR_PARITY_MASK      0x00000038  /**< Parity mask */
#define XUARTPSS_MR_CHARLEN_6_BIT    0x00000006  /**< 6 bits data */
#define XUARTPSS_MR_CHARLEN_7_BIT    0x00000004  /**< 7 bits data */
#define XUARTPSS_MR_CHARLEN_8_BIT    0x00000000  /**< 8 bits data */
#define XUARTPSS_MR_CHARLEN_SHIFT    1	   /**< data Length setting shift */
#define XUARTPSS_MR_CHARLEN_MASK     0x00000006  /**< Data length mask. */
#define XUARTPSS_MR_CLKSEL           0x00000001  /**< Input clock selection */
/* @} */


/** @name Interrupt Registers
 *
 * Interrupt control logic uses the interrupt enable register (IER) and the
 * interrupt disable register (IDR) to set the value of the bits in the
 * interrupt mask register (IMR). The IMR determines whether to pass an
 * interrupt to the interrupt status register (ISR).
 * Writing a 1 to IER Enbables an interrupt, writing a 1 to IDR disables an
 * interrupt. IMR and ISR are read only, and IER and IDR are write only.
 * Reading either IER or IDR returns 0x00.
 *
 * All four registers have the same bit definitions.
 *
 * @{
 */

#define XUARTPSS_IXR_DMS     0x00000200	/**< Modem status change interrupt */
#define XUARTPSS_IXR_TOUT    0x00000100	/**< Timeout error interrupt */
#define XUARTPSS_IXR_PARITY  0x00000080	/**< Parity error interrupt */
#define XUARTPSS_IXR_FRAMING 0x00000040	/**< Framing error interrupt */
#define XUARTPSS_IXR_OVER    0x00000020	/**< Overrun error interrupt */
#define XUARTPSS_IXR_TXFULL  0x00000010	/**< TX FIFO full interrupt. */
#define XUARTPSS_IXR_TXEMPTY 0x00000008	/**< TX FIFO empty interrupt. */
#define XUARTPSS_IXR_RXFULL  0x00000004	/**< RX FIFO full interrupt. */
#define XUARTPSS_IXR_RXEMPTY 0x00000002	/**< RX FIFO empty interrupt. */
#define XUARTPSS_IXR_RXOVR   0x00000001	/**< RX FIFO trigger interrupt. */
#define XUARTPSS_IXR_MASK    0x000003FF	/**< Valid bit mask */
/* @} */


/** @name Baud Rate Generator Register
 *
 * The baud rate generator control register (BRGR) is a 16 bit register that
 * controls the receiver bit sample clock and baud rate.
 * Valid values are 1 - 65535.
 *
 * Bit Sample Rate = CCLK / BRGR, where the CCLK is selected by the MR_CCLK bit
 * in the MR register.
 * @{
 */
#define XUARTPSS_BAUDGEN_DISABLE 0x00000000    /**< Disable clock */
#define XUARTPSS_BAUDGEN_MASK    0x0000FFFF	   /**< Valid bits mask */

/** @name Baud Divisor Rate register
 *
 * The baud rate divider register (BDIV) controls how much the bit sample
 * rate is divided by. It sets the baud rate.
 * Valid values are 0x04 to 0xFF. Writing a value less than 4 will be ignored.
 *
 * Baud rate = CCLK / ((BAUDDIV + 1) x BRGR), where the CCLK is selected by
 * the MR_CCLK bit in the MR register.
 * @{
 */
#define XUARTPSS_BAUDDIV_MASK  0x000000FF    /**< 8 bit baud divider mask */
/* @} */


/** @name Receiver Timeout Register
 *
 * Use the receiver timeout register (RTR) to detect an idle condition on
 * the receiver data line.
 *
 * @{
 */
#define XUARTPSS_RXTOUT_DISABLE    0x00000000  /**< Disable time out */
#define XUARTPSS_RXTOUT_MASK       0x000000FF  /**< Valid bits mask */

/** @name Receiver FIFO Trigger Level Register
 *
 * Use the Receiver FIFO Trigger Level Register (RTRIG) to set the value at
 * which the RX FIFO triggers an interrupt event.
 * @{
 */

#define XUARTPSS_RXWM_DISABLE 0x00000000  /**< Disable RX trigger interrupt */
#define XUARTPSS_RXWM_MASK    0x0000001F  /**< Valid bits mask */
/* @} */

/** @name Modem Control Register
 *
 * This register (MODEMCR) controls the interface with the modem or data set,
 * or a peripheral device emulating a modem.
 *
 * @{
 */
#define XUARTPSS_MODEMCR_FCM 0x00000010	 /**< Flow control mode */
#define XUARTPSS_MODEMCR_RTS 0x00000002	 /**< Request to send */
#define XUARTPSS_MODEMCR_DTR 0x00000001	 /**< Data terminal ready */
/* @} */

/** @name Modem Status Register
 *
 * This register (MODEMSR) indicates the current state of the control lines
 * from a modem, or another peripheral device, to the CPU. In addition, four
 * bits of the modem status register provide change information. These bits
 * are set to a logic 1 whenever a control input from the modem changes state.
 *
 * Note: Whenever the DCTS, DDSR, TERI, or DDCD bit is set to logic 1, a modem
 * status interrupt is generated and this is reflected in the modem status
 * register.
 *
 * @{
 */
#define XUARTPSS_MODEMSR_FCMS  0x00000100  /**< Flow control mode (FCMS) */
#define XUARTPSS_MODEMSR_DCD   0x00000080  /**< Complement of DCD input */
#define XUARTPSS_MODEMSR_RI    0x00000040  /**< Complement of RI input */
#define XUARTPSS_MODEMSR_DSR   0x00000020  /**< Complement of DSR input */
#define XUARTPSS_MODEMSR_CTS   0x00000010  /**< Complement of CTS input */
#define XUARTPSS_MEDEMSR_DCDX  0x00000008  /**< Delta DCD indicator */
#define XUARTPSS_MEDEMSR_RIX   0x00000004  /**< Change of RI */
#define XUARTPSS_MEDEMSR_DSRX  0x00000002  /**< Change of DSR */
#define XUARTPSS_MEDEMSR_CTSX  0x00000001  /**< Change of CTS */
/* @} */

/** @name Channel Status Register
 *
 * The channel status register (CSR) is provided to enable the control logic
 * to monitor the status of bits in the channel interrupt status register,
 * even if these are masked out by the interrupt mask register.
 *
 * @{
 */

#define XUARTPSS_SR_FLOWDEL  0x00001000	 /**< RX FIFO fill over flow delay */
#define XUARTPSS_SR_TACTIVE  0x00000800	 /**< TX active */
#define XUARTPSS_SR_RACTIVE  0x00000400	 /**< RX active */
#define XUARTPSS_SR_DMS      0x00000200	 /**< Delta modem status change */
#define XUARTPSS_SR_TOUT     0x00000100	 /**< RX timeout */
#define XUARTPSS_SR_PARITY   0x00000080	 /**< RX parity error */
#define XUARTPSS_SR_FRAME    0x00000040	 /**< RX frame error */
#define XUARTPSS_SR_OVER     0x00000020	 /**< RX overflow error */
#define XUARTPSS_SR_TXFULL   0x00000010	 /**< TX FIFO full */
#define XUARTPSS_SR_TXEMPTY  0x00000008	 /**< TX FIFO empty */
#define XUARTPSS_SR_RXFULL   0x00000004	 /**< RX FIFO full */
#define XUARTPSS_SR_RXEMPTY  0x00000002	 /**< RX FIFO empty */
#define XUARTPSS_SR_RXOVR    0x00000001	 /**< RX FIFO fill over trigger */
/* @} */

/** @name Flow Delay Register
 *
 * Operation of the flow delay register (FLOWDEL) is very similar to the
 * receive FIFO trigger register. An internal trigger signal activates when the
 * FIFO is filled to the level set by this register. This trigger will not
 * cause an interrupt, although it can be read through the channel status
 * register. In hardware flow control mode, RTS is deactivated when the trigger
 * becomes active. RTS only resets when the FIFO level is four less than the
 * level of the flow delay trigger and the flow delay trigger is not activated.
 * A value less than 4 disables the flow delay.
 * @{
 */
#define XUARTPSS_FLOWDEL_MASK    XUARTPSS_RXWM_MASK    /**< Valid bit mask */
/* @} */

/**************************** Type Definitions *******************************/


/***************** Macros (Inline Functions) Definitions *********************/

/****************************************************************************/
/**
* Read a UART register.
*
* @param    BaseAddress contains the base address of the device.
* @param    RegOffset contains the offset from the base address of the device.
*
* @return   The value read from the register.
*
* @note
* C-Style signature:
*    u32 XUartPss_ReadReg(u32 BaseAddress. int RegOffset)
*
******************************************************************************/
#define XUartPss_ReadReg(BaseAddress, RegOffset) \
    Xil_In32((BaseAddress) + (RegOffset))

/***************************************************************************/
/**
* Write a UART register.
*
* @param    BaseAddress contains the base address of the device.
* @param    RegOffset contains the offset from the base address of the device.
* @param    RegisterValue is the value to be written to the register.
*
* @return   None.
*
* @note
* C-Style signature:
*    void XUartPss_WriteReg(u32 BaseAddress, int RegOffset,
*                           u16 RegisterValue)
******************************************************************************/
#define XUartPss_WriteReg(BaseAddress, RegOffset, RegisterValue) \
    Xil_Out32((BaseAddress) + (RegOffset), (RegisterValue))

/****************************************************************************/
/**
* Determine if there is receive data in the receiver and/or FIFO.
*
* @param    BaseAddress contains the base address of the device.
*
* @return   TRUE if there is receive data, FALSE otherwise.
*
* @note
*
* C-Style signature:
*    u32 XUartPss_IsReceiveData(u32 BaseAddress)
*
******************************************************************************/
#define XUartPss_IsReceiveData(BaseAddress)                                 \
    !((Xil_In32((BaseAddress) + XUARTPSS_SR_OFFSET) & \
    XUARTPSS_SR_RXEMPTY) == XUARTPSS_SR_RXEMPTY)

/****************************************************************************/
/**
* Determine if a byte of data can be sent with the transmitter.
*
* @param    BaseAddress contains the base address of the device.
*
* @return   TRUE if the TX FIFO is full, FALSE if a byte can be put in the
*            FIFO.
*
* @note
* C-Style signature:
*    u32 XUartPss_IsTransmitFull(u32 BaseAddress)
*
******************************************************************************/
#define XUartPss_IsTransmitFull(BaseAddress)                         \
    ((Xil_In32((BaseAddress) + XUARTPSS_SR_OFFSET) & \
     XUARTPSS_SR_TXFULL) == XUARTPSS_SR_TXFULL)



/************************** Function Prototypes ******************************/

void XUartPss_SendByte(u32 BaseAddress, u8 Data);

u8 XUartPss_RecvByte(u32 BaseAddress);
/************************** Variable Definitions *****************************/

#ifdef __cplusplus
}
#endif

#endif /* end of protection macro */
