/*
 * eusci_ser0_config.h
 *
 * This file contains the configuration information for the EUSCI Ser0,
 * which is the first EUSCI-based serial port in the system.
 *
 * Note that the lwevent structure here is pretty flexible: the handler
 * functions (has_data and is_empty) are overridable, as are the lwevents
 * themselves.
 *
 * To do something clever, like one lwevent for all serial ports in the
 * system, you can instantiate a single lwevent, and use symbol remapping
 * (--symbol_map='EUSCI_Ser1_tx_empty_event=EUSCI_Ser0_tx_empty_event')
 * to allow the second symbol to resolve the first.
 *
 * Note the pointers, lwevents, and buffers are all C-type objects
 * to allow the assembly ISR to interact without worrying about C++
 * name mangling, and also allows symbol remapping to be done at the C
 * rather than C++ level.
 *
 *  Created on: Jul 1, 2015
 *      Author: barawn
 */

#ifndef EUSCI_SER0_CONFIG_H_
#define EUSCI_SER0_CONFIG_H_

#include <stdint.h>
#include <msp430.h>
#include "core/uarts/uart_base.h"

#define UART_OBJECT( x ) EUSCI_Ser0_##x
//% \brief This is required for the ISR, since there's no way to include that in the Config class. Make sure BASE and VECTOR_LOCATION match!
#define EUSCI_Ser0_VECTOR_LOCATION	USCI_A0_VECTOR

//% \brief Actual ring buffer pointers. Pointed to inside config class.
extern uart_base_pointers UART_OBJECT(pointers);
//% \brief Actual ring buffer RX storage. Pointed to inside config class.
extern uint8_t UART_OBJECT(rx_buffer[]);
//% \brief Actual ring buffer TX storage. Pointed to inside config class.
extern uint8_t UART_OBJECT(tx_buffer[]);
//% \brief Actual TX empty lwevent. Overridable.
extern lwevent UART_OBJECT(tx_empty_event);
//% \brief Actual RX data lwevent. Overridable.
extern lwevent UART_OBJECT(rx_data_event);
//% \brief Actual tx_empty store. Overridable.
extern lwevent_store_fifo UART_OBJECT(tx_empty_store);

//% \brief Configuration class for EUSCI_Ser0.
//%
//% Picking off the pointers is done to avoid MSP430 C compiler
//% stupidity (buf[*pidx] is more efficient than buf[p->idx]).
//% Collecting them in a class in the first place is done to reduce
//% the number of implementation-specific objects (e.g. EUSCI_Ser0_)
//% prefixed objects.
class EUSCI_Ser0_Config {
public:
	const uint8_t TX_BUF_SIZE = 128;
	const uint8_t RX_BUF_SIZE = 16;
	const uint16_t BASE = __MSP430_BASEADDRESS_EUSCI_A0__;
	const uint16_t CTLW0 = UCSSEL_2;
	const uint16_t BRW = 6;
	const uint16_t MCTLW = (0x20 * UCBRS0) + (0x8 * UCBRF0) + UCOS16;

	uart_base_pointers *const pointers = &UART_OBJECT(pointers);
	uint8_t *const rx_buffer = UART_OBJECT(rx_buffer);
	uint8_t *const tx_buffer = UART_OBJECT(tx_buffer);
	lwevent *const tx_empty_event = &UART_OBJECT(tx_empty_event);
	lwevent *const rx_data_event = &UART_OBJECT(rx_data_event);
	lwevent_store_fifo *const tx_empty_store = &UART_OBJECT(tx_empty_store);

	uint8_t *const rx_rd_ptr = &pointers->rx_rd;
	uint8_t *const rx_wr_ptr = &pointers->rx_wr;
	uint8_t *const tx_rd_ptr = &pointers->tx_rd;
	uint8_t *const tx_wr_ptr = &pointers->tx_wr;
};


//% Define this to exclude the default empty/data lwevents. Use this if you override the lwevents themselves. Note that NOT defining this still works,
//% but costs some RAM/code space. This is because the compiler can't tell that that event is unused, because it looks like the handler accesses it,
//% and the event also contains the handler. So it looks like the usage count of both is 1.
//#define EUSCI_SER0_NO_DEFAULT_LWEVENTS

//% \brief Define this to exclude the default tx_empty lwevent (automatically excluded if SER0_NO_DEFAULT_LWEVENTS is set)
//#define EUSCI_SER0_NO_DEFAULT_TX_EMPTY_LWEVENT

//% \brief Define this to exclude the default RX data lwevent (automatically excluded if SER0_NO_DEFAULT_LWEVENTS is set)
//#define EUSCI_SER0_NO_DEFAULT_RX_DATA_LWEVENT

//% \brief Convenience enum to work around bugs in cdecls parser.
enum UART_OBJECT(Config_enum) {
	UART_OBJECT(BASE) = UART_OBJECT(Config::BASE),
	UART_OBJECT(TX_BUF_SIZE) = UART_OBJECT(Config::TX_BUF_SIZE),
	UART_OBJECT(RX_BUF_SIZE) = UART_OBJECT(Config::RX_BUF_SIZE)
};

#undef UART_OBJECT

#endif /* EUSCI_SER0_CONFIG_H_ */
