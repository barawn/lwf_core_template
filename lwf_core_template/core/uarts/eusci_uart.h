/*
 * eusci_uart.h
 *
 *  Created on: Jun 29, 2015
 *      Author: barawn
 */

#ifndef EUSCI_UART_H_
#define EUSCI_UART_H_

#include <msp430.h>
#include "uart_base.h"

// All MSP430s with EUSCIs have these defined.
#ifndef __MSP430_HAS_EUSCI_A0__
#error EUSCI_UART not found in selected device.
#else
#define EUSCI_REG( x ) (*(&(x) + CONFIG::BASE - __MSP430_BASEADDRESS_EUSCI_A0__))
#endif

template<typename CONFIG>
class EUSCI_UART : public uart_base< EUSCI_UART<CONFIG>, CONFIG > {
public:
	EUSCI_UART() {}
	static void init_impl() {
		EUSCI_REG(UCA0CTLW0) = CONFIG::CTLW0 | UCSWRST;
		EUSCI_REG(UCA0BRW) = CONFIG::BRW;
		EUSCI_REG(UCA0MCTLW) = CONFIG::MCTLW;
		EUSCI_REG(UCA0CTLW0) &= ~UCSWRST;
	}
	static void control_impl(bool enable) {
		if (enable) EUSCI_REG(UCA0IE) |= UCRXIE;
		else EUSCI_REG(UCA0IE) &= ~UCRXIE;
	}
	static bool sleep_check_impl() {
		// is the interrupt running?
		if (!(EUSCI_REG(UCA0IE) & UCTXIE)) {
			// No. So is there anything available?
			if (*(EUSCI_UART::get_tx_rd_ptr()) !=
				*(EUSCI_UART::get_tx_wr_ptr())) {
				// Yes, so enable the interrupt.
				EUSCI_REG(UCA0IE) |= UCTXIE;
			}
		}
		return true;
	}
	// This is here because of weirdness: I can't redeclare
	// a static member outside of the class, but I can't
	// include attributes in the actual function, so the
	// only way is to change the class definition that
	// each file sees.
	//
	// Here, the library-level code (COMPONENTS_DEFAULT)
	// sees 'weak' attributes, so their defaults can be overriden.
	// Everyone else sees strong, so their functions override.
#ifdef COMPONENTS_DEFAULT
	static void is_empty(lwevent *ev) __attribute__((weak));
	static void has_data(lwevent *ev) __attribute__((weak));
#else
	static void is_empty(lwevent *ev);
	static void has_data(lwevent *ev);
#endif
};

#endif /* EUSCI_UART_H_ */
