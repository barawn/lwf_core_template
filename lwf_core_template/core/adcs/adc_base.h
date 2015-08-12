/*
 * adc_base.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef _ADC_BASE_H_
#define _ADC_BASE_H_

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
		return (HW::waiting_lwevent == lwevent::LWEVENT_WAITING);
	}
	static void prepare(lwevent *ev) {
		HW::waiting_lwevent = ev;
	}
	static void convert(uint8_t ch) {
		HW::convert_impl(ch);
	}
	static void convert() {
		HW::convert_impl();
	}
	static void release() {
		lwevent *t;
		HW::waiting_lwevent = lwevent::LWEVENT_WAITING;
		if (HW::wait_queue.empty()) return;
		t = HW::wait_queue.pop();
		t->handler(t);
	}
	static void wait_for_available(lwevent *t) {
		if (HW::waiting_lwevent == lwevent::LWEVENT_WAITING) {
			t->handler(t);
		} else {
			HW::wait_queue.store(t);
		}
	}
};



#endif /* _ADC_BASE_H_ */
