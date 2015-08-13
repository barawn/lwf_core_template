/*
 * adc_base.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef ADC_BASE_H_
#define ADC_BASE_H_

#include "../lwevent.h"
//% Base class for a sharable ADC.
//%
//% This class contains the framework for a sharable ADC implementation,
//% like the I2C implementation. You call available() to find out if the ADC
//% is in use, you call prepare() once it is, and then you call convert()
//% to actually perform the conversion. Once the conversion is done, the lwevent
//% passed to prepare has its handler called.
template <class HW, class CONFIG>
class adc_base {
public:
	typedef typename CONFIG::adc_result_type result_type;
	void init() {
		HW::init_impl();
	}
	result_type get_value() {
		return HW::get_value_impl();
	}
	static bool available() {
		return (*waiting_lwevent_p == lwevent::LWEVENT_WAITING);
	}
	static void prepare(lwevent *ev) {
		*waiting_lwevent_p = ev;
	}
	static void convert(uint8_t ch) {
		HW::convert_impl(ch);
	}
	static void convert() {
		HW::convert_impl();
	}
	static void release() {
		lwevent *t;
		*waiting_lwevent_p = lwevent::LWEVENT_WAITING;
		if (HW::wait_queue.empty()) return;
		t = HW::wait_queue.pop();
		t->handler(t);
	}
	static void wait_for_available(lwevent *t) {
		if (*waiting_lwevent_p == lwevent::LWEVENT_WAITING) {
			t->handler(t);
		} else {
			HW::wait_queue.store(t);
		}
	}
	static void helper_handler(lwevent *ev) {
		ev->clear();
		(*waiting_lwevent_p)->handler(*waiting_lwevent_p);
	}
	static lwevent_store_fifo wait_queue;
	lwevent **const waiting_lwevent_p = CONFIG::waiting_lwevent_p;
};



#endif /* ADC_BASE_H_ */
