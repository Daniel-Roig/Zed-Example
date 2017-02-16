#include "xparameters.h"
#include "xil_types.h"
#include "xil_assert.h"
#include "xil_io.h"
#include "ps7_init.h"

/* Register offsets */
#define UART_CR_OFFSET		0x00
#define UART_MR_OFFSET		0x04
#define UART_BAUDGEN_OFFSET	0x18
#define UART_BAUDDIV_OFFSET	0x34

#define MAX_BAUD_ERROR_RATE	3	/* max % error allowed */

#define UART_BAUD_RATE  115200

void InitUART(void)
{
	u8 IterBAUDDIV;		/* Iterator for available baud divisor values */
	u32 BRGR_Value;		/* Calculated value for baud rate generator */
	u32 CalcBaudRate;	/* Calculated baud rate */
	u32 BaudError;		/* Diff between calculated and requested baud
				 * rate */
	u32 Best_BRGR = 0;	/* Best value for baud rate generator */
	u8 Best_BAUDDIV = 0;	/* Best value for baud divisor */
	u32 Best_Error = 0xFFFFFFFF;
	u32 PercentError;
	u32 InputClk;
    u32 BaudRate = UART_BAUD_RATE;

#ifdef UART_FREQ		/* PCW input */
	InputClk = UART_FREQ;
#else
#ifdef UART_CLOCK
	InputClk = UART_CLOCK;
#else
#if (STDOUT_BASEADDRESS == XPAR_XUARTPSS_0_BASEADDR)
	InputClk = XPAR_XUARTPSS_0_CLOCK_HZ;
#elif (STDOUT_BASEADDRESS == XPAR_XUARTPSS_1_BASEADDR)
	InputClk = XPAR_XUARTPSS_1_CLOCK_HZ;
#else
	/* STDIO is not set or axi_uart is being used for STDIO */
	return;
#endif
#endif
#endif

#ifndef PALLADIUM
	/*
	 * Determine the Baud divider. It can be 4to 254.
	 * Loop through all possible combinations
	 */
	for (IterBAUDDIV = 4; IterBAUDDIV < 255; IterBAUDDIV++) {

		/*
		 * Calculate the value for BRGR register
		 */
		BRGR_Value = InputClk / (BaudRate * (IterBAUDDIV + 1));

		/*
		 * Calculate the baud rate from the BRGR value
		 */
		CalcBaudRate = InputClk/ (BRGR_Value * (IterBAUDDIV + 1));

		/*
		 * Avoid unsigned integer underflow
		 */
		if (BaudRate > CalcBaudRate) {
			BaudError = BaudRate - CalcBaudRate;
		} else {
			BaudError = CalcBaudRate - BaudRate;
		}

		/*
		 * Find the calculated baud rate closest to requested baud rate.
		 */
		if (Best_Error > BaudError) {

			Best_BRGR = BRGR_Value;
			Best_BAUDDIV = IterBAUDDIV;
			Best_Error = BaudError;
		}
	}

	/*
	 * Make sure the best error is not too large.
	 */
	PercentError = (Best_Error * 100) / BaudRate;
	if (MAX_BAUD_ERROR_RATE < PercentError) {
		return;
	}

	/* set CD and BDIV */
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDGEN_OFFSET, Best_BRGR);
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDDIV_OFFSET, Best_BAUDDIV);
#else
    /*  Baudrate calculation:
    *   bus_rate / (baudgen * (bauddiv + 1) * 4)
    *   For palladium baudgen and bauddiv configured with constant value
    *       baudgen = 0x4
    *       bauddiv = 0x4
    */  
    
    /* set CD and BDIV */
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDGEN_OFFSET, 0x14);
	Xil_Out32(STDOUT_BASEADDRESS + UART_BAUDDIV_OFFSET, 0x4);
#endif

	/*
	 * 8 data, 1 stop, 0 parity bits
	 * sel_clk=uart_clk=APB clock
	 */
	Xil_Out32(STDOUT_BASEADDRESS + UART_MR_OFFSET, 0x20);

	/* enable Tx/Rx and reset Tx/Rx data path */
	Xil_Out32((STDOUT_BASEADDRESS + UART_CR_OFFSET), 0x17);

	return;
}

