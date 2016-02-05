/*
 * wdt_system_clock.h
 *
 * wdt clocks require a queue0, for the ISR. It's only used in the ISR, but we
 * include it here for good measure.
 *
 *  Created on: Aug 5, 2015
 *      Author: barawn
 */

#ifndef WDT_SYSTEM_CLOCK_H_
#define WDT_SYSTEM_CLOCK_H_

#include "system_clock_base.h"
#include <msp430.h>

#if !defined(__MSP430_HAS_WDT__) and !defined(__MSP430_HAS_WDT_A__)
#error WDT not found in selected device
#endif

// TI didn't do a compatibility define, so I will.
#ifdef __MSP430_HAS_WDT__
#define WDT_IE IE1
#define WDT_IFG IFG1
#else
#define WDT_IE SFRIE1
#define WDT_IFG SFRIFG1
#endif

extern isr_lwevent_queue queue0;

template<typename CONFIG>
class WDT_system_clock : public system_clock_base<WDT_system_clock<CONFIG>, CONFIG> {
public:
	WDT_system_clock() {}
	static void init_impl() {
		// This is always called WDTCTL.
		WDTCTL = WDTPW | CONFIG::CTL | WDTTMSEL | WDTCNTCL;

		// This is sometimes called SFRIFG1, and sometimes IFG1, and they didn't do a compatibility define.
		WDT_IFG &= ~WDTIFG;
		WDT_IE |= WDTIE;
	}
	lwevent *const tick_lwevent = CONFIG::tick_lwevent;

	static uint16_t last_count;
	static bool phase;
	static lwevent_store_fifo store0;
	static lwevent_store_fifo store1;
};

#endif /* WDT_SYSTEM_CLOCK_H_ */
