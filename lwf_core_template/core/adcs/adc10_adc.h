/*
 * adc10_adc.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef ADC10_ADC_H_
#define ADC10_ADC_H_

#include <msp430.h>
#include "adc_base.h"

// This CURRENTLY only supports an ADC10. I need to check on what's required
// for an ADC10_A or ADC10_B.
#ifndef __MSP430_HAS_ADC10__
#error ADC10 not found in selected device
#endif



template<class CONFIG>
class ADC10_ADC : public adc_base<ADC10_ADC<CONFIG>, CONFIG> {
public:
	typedef typename CONFIG::adc_result_type result_type;
	static void init_impl() {
		ADC10CTL0 = CONFIG::CTL0;
		ADC10CTL1 = CONFIG::CTL1;
	}
	static void convert_impl(uint8_t ch) {
		uint16_t tmp;
		ADC10CTL0 &= ~ENC;
		tmp = (ch) << 12;
		ADC10CTL1 &= ~INCH_15;
		ADC10CTL1 |= tmp;
		ADC10_ADC<CONFIG>::convert_impl();
	}
	static void convert_impl() {
		// Conversion with no channel switching.
		ADC10CTL0 |= ENC;
		ADC10CTL0 |= ADC10SC | ADC10IE;
		// Interrupt will post the lwevent.
		return;
	}
	result_type get_value() {
		return ADC10MEM;
	}
	static lwevent_store_fifo wait_queue;
};

#endif /* ADC10_ADC_H_ */
