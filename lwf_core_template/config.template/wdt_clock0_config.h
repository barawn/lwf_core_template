/*
 * wdt_clock0_config.h
 *
 *  Created on: Aug 5, 2015
 *      Author: barawn
 */

#ifndef WDT_CLOCK0_CONFIG_H_
#define WDT_CLOCK0_CONFIG_H_

#include <stdint.h>
#include <msp430.h>
#include "core/clocks/system_clock_base.h"

#define CLOCK_OBJECT(x) WDT_Clock0_##x
#define WDT_Clock0_VECTOR_LOCATION		WDT_VECTOR

extern volatile uint16_t CLOCK_OBJECT(count);
extern lwevent CLOCK_OBJECT(tick_lwevent);

/* \brief WDT_Clock0 config object.
 *
 * - SMCLK based.
 * - 1 MHz SMCLK + 32768 SMCLKs per tick = 30 counts per second-ish.
 *   (32 ms per tick).
 *
 */
class CLOCK_OBJECT(Config) {
public:
	const uint16_t CTL = WDT_MDLY_32;
	const uint16_t ticks_per_second = 30;
	volatile uint8_t *const IE = &IE1;
	volatile uint8_t *const IFG = &IFG1;
	volatile uint16_t *const count = &CLOCK_OBJECT(count);
	lwevent *const tick_lwevent = &CLOCK_OBJECT(tick_lwevent);
};


#endif /* WDT_CLOCK0_CONFIG_H_ */
