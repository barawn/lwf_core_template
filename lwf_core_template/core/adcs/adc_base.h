/*
 * adc_base.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef ADC_BASE_H_
#define ADC_BASE_H_

#include "../lwevent.h"
//% Base class for ADCs.
//%
//% Base class for ADC implementations. ADCs are a lot like I2C
//% implementations, in that they can be shared (or not shared).
//% First you check if it's available, then call prepare to claim
//% it and set up the conversion, and then call convert to actually
//% perform the conversion.
//%
//% If the ADC is not available, you call wait_for_available()
//% and the lwevent will then post when the ADC becomes available.
//%
//% convert() with no argument avoids any channel-switching for
//% repeated conversions.
//%
//% ADCs differ from I2C implementations in that the interrupt
//% handler posts the waiting lwevent directly, since there isn't any
//% 'cleanup' needed (typically - this could of course be handled
//% in the HW phase by having the ISR post a different lwevent with
//% a local handler).
//%
//% Therefore 'prepare' consists of passing the *callback*, so
//% that the *internal* lwevent can be posted.
template <class HW, class CONFIG>
class adc_base {
public:
	void init() {
		HW::init_impl();
	}
	CONFIG::adc_result_type get_value() {
		return HW::get_value_impl();
	}
	static bool available() {
		return (waiting_lwevent->next = lwevent::LWEVENT_DISABLED);
	}
	static void prepare(lwevent_handler_t callback) {
		waiting_lwevent->handler = callback;
	}
	static void convert(uint8_t ch) {
		HW::convert_impl(ch);
	}
	static void convert() {
		HW::convert_impl();
	}
	static void release() {
		lwevent *t;
		waiting_lwevent->next = lwevent::LWEVENT_DISABLED;
		if (HW::wait_queue.empty()) return;
		t = HW::wait_queue.pop();
		t->handler(t);
	}
	static void wait_for_available(lwevent *t) {
		if (waiting_lwevent->next == lwevent::LWEVENT_DISABLED) {
			t->handler(t);
		} else {
			HW::wait_queue.store(t);
		}
	}

	lwevent *const waiting_lwevent = CONFIG::waiting_lwevent;
};



#endif /* ADC_BASE_H_ */
