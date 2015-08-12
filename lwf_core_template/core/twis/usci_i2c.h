/*
 * usci_i2c_base.h
 *
 * Basic USCI-B I2C implementation.
 * Note: this *requires* a queue0/queue1 be implemented
 * in the system. queue0 is only used in the ISR. It's included here
 * to make it explicit.
 *
 * This implementation doesn't allow for repeated starts: all transactions are fully-framed
 * with S/P. Repeated starts could be implemented by not having the ISR issue UCTXSTP at the
 * final byte, but rather just waking up. Then the lwevent handler would have to know whether
 * to issue UCTXSTT/UCTXSTP, which would be done by checking I2C_TYPE_REPEATED,
 * and if it's set, set UCTXSTT instead, and then call complete(). Something like that.
 *
 *  Created on: Jul 24, 2015
 *      Author: barawn
 */

#ifndef USCI_I2C_H_
#define USCI_I2C_H_

#include <msp430.h>
#include "i2c_base.h"

extern isr_lwevent_queue queue0;
extern lwevent_queue queue1;

#if not defined(__MSP430_HAS_USCI_B0__) && not defined(__MSP430_HAS_USCI__)
#error USCI_UART not found in selected device.
#else
#if defined(__MSP430_BASEADDRESS_USCI_B0__)
#define USCI_BASE ((const uint16_t) __MSP430_BASEADDRESS_USCI_B0__)
#else
#define USCI_BASE ((const uint16_t) &UCB0CTL0)
#endif
#define USCI_OFFSET(x) ((const uint16_t) (((const uint16_t)&x) - USCI_BASE))
#define USCI_REG8( x ) (*((volatile uint8_t *const) (USCI_OFFSET(x) + (const uint16_t) CONFIG::BASE)))
#define USCI_REG16( x ) (*((volatile uint16_t *const) (USCI_OFFSET(x) + (const uint16_t) CONFIG::BASE)))
#endif


template<class CONFIG>
class USCI_I2C : public i2c_base<USCI_I2C<CONFIG>, CONFIG> {
public:
	typedef i2c_base<USCI_I2C<CONFIG>,CONFIG> I2C;
	USCI_I2C() {}
	static void init_impl() {
		USCI_REG8(UCB0CTL1) |= UCSWRST;
		USCI_REG8(UCB0CTL1) = CONFIG::CTL1;
		USCI_REG8(UCB0CTL0) = CONFIG::CTL0;
		USCI_REG8(UCB0BR0) = CONFIG::BR0;
		USCI_REG8(UCB0BR1) = CONFIG::BR1;
		USCI_REG8(UCB0CTL1) &= ~UCSWRST;
	}
	//% \brief Actually submit the transaction.
	//%
	//% Note: address, len, and dirtype are passed here rather than embedded in
	//% the transaction because they're constants inside here. Since this function gets inlined,
	//% it actually can figure out all of the different branches below at compile time
	//% if the "submit" arguments are constant, ending up with a maximally compact
	//% function.
	//%
	//% In fact if you do prepare(), submit(), it's smart enough to optimize a good
	//% portion of what's required away.
	static void submit_impl(uint8_t address, uint8_t len, i2c_dirtype_t dirtype) {
		uint8_t to_set_ie = 0;

		transaction->status |= I2C::I2C_STATUS_BUSY;
		// set up the slave address
		USCI_REG8(UCB0I2CSA) = address;
		// set up the data pointer
		*CONFIG::isr_data_ptr = transaction->data;
		// set up the direction of transfer
		if (dirtype == I2C_DIRECTION_READ) {
			uint8_t tmp;

			USCI_REG8(UCB0CTL1) &= ~UCTR;
			// Enable the RX interrupt. The RX interrupt will
			// self-disable when isr_count < 0 (after decrement).
			// The RX interrupt also sends UCTXSTP when isr_count equals 0
			// after decrement.

			// Number of ADDITIONAL reads.
			// So for a read of 1 byte, it detects isr_count < 0, disables itself,
			// and wakes up.
			*CONFIG::isr_count = len - 1;
			if (len == 1) {
				// Special case: for reads of 1 byte, after we issue UCTXSTT, we need
				// to poll for it to clear.
				//
				// We *CANNOT* enable interrupts, in this case!

				// We're now in "polling start" status.
				transaction->status |= I2C::I2C_STATUS_POLLING_START;
				// We know the state lwevent is clear.
				queue1.post_without_check(CONFIG::state_lwevent);
				*CONFIG::isr_data_ptr = transaction->data;
				USCI_REG8(UCB0CTL1) |= UCTXSTT;

				// It will then poll (fairly, sharing time with everything else)
				// until UCTXSTT clears, which indicates that we've gotten to the ACK
				// portion, and then send STP.
				//
				// Polling fairly is important here: it's 9 bit times! At 100 kbps,
				// this is ~90 us. At *slower* bitrates, it's obviously even worse!
				return;
			}
			to_set_ie = CONFIG::RXIE;
		}
		else {
			USCI_REG8(UCB0CTL1) |= UCTR;
			// Number of bytes to transmit.
			// So for a write of 1 byte, it decrements, sees = 0, sends UCTXSTP,
			// and wakes up. Software then polls for UCTXSTP to complete since there
			// is no interrupt we can get.
			*CONFIG::isr_count = len;
			to_set_ie = CONFIG::TXIE;
		}
		USCI_REG8(UCB0I2CIE) |= (UCNACKIE | UCALIE);
		// initiate it. ISR takes over after this.
		USCI_REG8(UCB0CTL1) |= UCTXSTT;
		*CONFIG::IE = to_set_ie;
	}
	static void i2c_lwevent_handler(lwevent *ev) {
		uint8_t tmp;
		uint8_t tmp2;
		// clear event
		ev->clear();

		////////////////////////////////
		// The USCI lwevent handler deals with the
		// portions of the I2C transaction that can't be handled
		// in the ISR. These include:
		// 1) Polling for stop to complete at the end of the transaction
		// 2) Polling for UCTXSTT to clear to issue a STOP after the address
		//    byte in the case of a read.
		//
		// In a normal transaction (write, or >1 read bytes)
		// 1) it gets called once when the transaction completes.
		//    At that point UCTXSTP is set, because it has not been transmitted
		//    and we do not get an interrupt for that.
		// 2) It gets repetitively called (in the *non* ISR queue) until
		//    UCTXSTP clears. Anything else that posts will get similarly serviced
		//    too.
		//
		// In a read=1 byte transaction,
		// 1) it gets called immediately after UCTXSTT (well, after the lwevent queue
		//    posts it)
		// 2) it gets repetitively called until UCTXSTT clears.

		// what's going on? are we polling?
		tmp = USCI_REG8(UCB0CTL1);
		tmp2 = USCI_REG8(UCB0STAT);
		if (transaction->status & I2C::I2C_STATUS_POLLING_START) {
			if (tmp2 & UCALIFG) {
				// Arbitration lost: so UCTXSTT stays set. We need to clear it.
				USCI_REG8(UCB0CTL1) &= ~UCTXSTT;
				USCI_REG8(UCB0STAT) &= ~UCALIFG;
				transaction->status |= I2C::I2C_STATUS_ARBITRATION_LOST;
				I2C::complete();
				return;
			}
			if (tmp & UCTXSTT) {
				// This is obviously clear.
				queue1.post_without_check(ev);
				return;
			}
			// Set STP.
			USCI_REG8(UCB0CTL1) |= UCTXSTP;
			// UCTXSTT cleared. Why? Did NACKIFG go off?
			if (tmp2 & UCNACKIFG) {
				// Clear it.
				USCI_REG8(UCB0STAT) &= ~UCNACKIFG;
				// Set status to I2C_STATUS_BUSY.
				transaction->status &= ~I2C::I2C_STATUS_POLLING_START;
				// Then we need to continue polling on stop.
				queue1.post_without_check(ev);
				return;
			}
			// UCTXSTT cleared, with no NACKIFG, and no ALIFG. We're now in a normal operation.
			transaction->status &= ~I2C::I2C_STATUS_POLLING_START;
			// Enable interrupts.
			*CONFIG::IE |= CONFIG::RXIE;
			USCI_REG8(UCB0I2CIE) |= (UCNACKIE | UCALIE);
			// Done.
			return;
			// If ALIFG becomes set durinng this process, it will immediately wake up,
			// clear UCTXSTT/UCTXSTP, and wake us up again.
		}
		if (transaction->status & I2C::I2C_STATUS_BUSY) {
			// If we're here, that means that the interrupts are disabled.
			// Is stop set?
			if (tmp & UCTXSTP) {
				queue1.post_without_check(ev);
				return;
			}
			// At this point, stop has cleared, so the transaction
			// is complete.
			if (tmp2 & (UCNACKIFG|UCALIFG)) {
				if (tmp2 & UCNACKIFG) transaction->status |= I2C::I2C_STATUS_NACK;
				if (tmp2 & UCALIFG) transaction->status |= I2C::I2C_STATUS_ARBITRATION_LOST;
				tmp2 = tmp2 & ~(UCNACKIFG|UCALIFG);
				USCI_REG8(UCB0STAT) = tmp2;
				// clear pending flags too
				*CONFIG::IFG &= ~(CONFIG::RXIFG | CONFIG::TXIFG);
			} else {
				transaction->status = I2C::I2C_STATUS_OK;
			}
			I2C::complete();
			return;
		}
	}

	static i2c_transaction *transaction;
	static bool lock;
	static lwevent_store_fifo wait_queue;
};



#endif /* USCI_I2C_H_ */
