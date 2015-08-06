/*
 * usci_twi0_config.h
 *
 *  Created on: Aug 3, 2015
 *      Author: barawn
 */

#ifndef USCI_TWI0_CONFIG_H_
#define USCI_TWI0_CONFIG_H_

#include <stdint.h>
#include <msp430.h>
#include "../i2c_base.h"

#define I2C_OBJECT(x) USCI_Twi0_##x
#define USCI_Twi0_RXTX_VECTOR_LOCATION	0
#define USCI_B0_TX_ISR					USCI_Twi0_RXTX_ISR
#define USCI_Twi0_FLAG_VECTOR_LOCATION	0
#define USCI_B0_RX_ISR					USCI_Twi0_FLAG_ISR

//% \brief Actual I2C state lwevent.
extern	lwevent I2C_OBJECT(state_lwevent);
//% \brief Actual ISR byte count.
extern	uint8_t I2C_OBJECT(isr_count);
//% \brief Actual data pointer.
extern	uint8_t *I2C_OBJECT(isr_data_ptr);

//% \brief USCI B0 config.
//%
//% Use SMCLK.
//% Divide by 10 for bitclock.
class I2C_OBJECT(Config) {
public:
	const uint8_t CTL0 = UCMST | (UCMODE_3) | UCSYNC;
	const uint8_t CTL1 = UCSSEL_3;
	const uint8_t BR0 = 10;
	const uint8_t BR1 = 0;
	const uint16_t BASE = (uint16_t) &UCB0CTL0;
	const uint8_t USCI_INDEX = 0;
	volatile uint8_t *const IE = &IE2;
	const uint8_t TXIE = UCB0TXIE;
	const uint8_t RXIE = UCB0RXIE;
	volatile uint8_t *const IFG = &IFG2;
	const uint8_t TXIFG = UCB0TXIFG;
	const uint8_t RXIFG = UCB0RXIFG;

	lwevent *const state_lwevent = &I2C_OBJECT(state_lwevent);
	uint8_t *const isr_count = &I2C_OBJECT(isr_count);
	uint8_t **const isr_data_ptr = &I2C_OBJECT(isr_data_ptr);
};

//% Convenience define to work around bugs in cdecls parser.
#ifdef __ASM_HEADER__
enum I2C_OBJECT(Config_enum) {
	I2C_OBJECT(INDEX) = I2C_OBJECT(Config::USCI_INDEX),
};
#endif

#undef I2C_OBJECT

#endif /* USCI_TWI0_CONFIG_H_ */
