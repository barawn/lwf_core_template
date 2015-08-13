/*
 * adc10_adc0.cpp
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#define COMPONENTS_DEFAULT
// Add components.h. If we're added, we'll know.
// Otherwise, we don't add anything, and the cpp files do nothing.
#include "components.h"
// Change these lines for a new TWI. Everything else autorenames.
#ifdef ADC10_ADC0_H_

#define ADC_OBJECT( x ) ADC10_Adc0_##x
typedef ADC10_Adc0 This_ADC;

lwevent_store_fifo This_ADC::adc_base_type::wait_queue;
lwevent *ADC_OBJECT(waiting_lwevent_p) = (lwevent *) lwevent::LWEVENT_WAITING;
lwevent ADC_OBJECT(helper_lwevent)((lwevent_handler_t) This_ADC::adc_base_type::helper_handler, (lwevent *) lwevent::LWEVENT_WAITING);

#endif
