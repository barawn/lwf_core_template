/*
 * wdt_clock0.h
 *
 *  Created on: Aug 5, 2015
 *      Author: barawn
 */

#ifndef WDT_CLOCK0_H_
#define WDT_CLOCK0_H_

#include "wdt_system_clock.h"
#include "config/wdt_clock0_config.h"
typedef WDT_system_clock<WDT_Clock0_Config> WDT_Clock0;

#endif /* WDT_CLOCK0_H_ */
