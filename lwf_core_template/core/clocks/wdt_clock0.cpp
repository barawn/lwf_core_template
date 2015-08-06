/*
 * wdt_clock0.cpp
 *
 *  Created on: Aug 5, 2015
 *      Author: barawn
 */

#define COMPONENTS_DEFAULT
#include "../../components.h"

// Change these lines for a new WDT clock (how? WHO KNOWS). Everything else autorenames.
#ifdef WDT_CLOCK0_H_
#define CLOCK_OBJECT( x ) WDT_Clock0_##x
typedef WDT_Clock0 This_Clock;
/////////////////////////////
// Clock objects.
/////////////////////////////

volatile uint16_t CLOCK_OBJECT(count) = 0;
lwevent CLOCK_OBJECT(tick_lwevent) ((lwevent_handler_t) This_Clock::tick, (lwevent *) lwevent::LWEVENT_WAITING);

bool This_Clock::phase = false;
uint16_t This_Clock::last_count = 0;
lwevent_store_fifo This_Clock::store0;
lwevent_store_fifo This_Clock::store1;

#endif
