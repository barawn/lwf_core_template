/*
 * components.h
 *
 * components.h/components.cpp contain the lwframework components
 * used in this project.
 *
 *  Created on: Jul 1, 2015
 *      Author: barawn
 */

#ifndef COMPONENTS_H_
#define COMPONENTS_H_


// lwevent queues.
#include "core/lwevent.h"
extern isr_lwevent_queue queue0;
extern lwevent_queue queue1;

// Serial ports.
#include "core/uarts/usci_ser0.h"
typedef USCI_Ser0 Ser0;
extern Ser0 uart0;

// System clocks.
#include "core/clocks/wdt_clock0.h"
typedef WDT_Clock0 Clock0;
extern Clock0 clock0;

// Two-wire interfaces (I2C).
//#include "core/twis/usci_twi0.h"
//typedef USCI_Twi0 Twi0;
//extern Twi0 twi0;

void components_init();
void components_loop();

#endif /* COMPONENTS_H_ */
