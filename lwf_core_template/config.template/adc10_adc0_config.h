/*
 * adc10_adc0_config.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef ADC10_ADC0_CONFIG_H_
#define ADC10_ADC0_CONFIG_H_

#define ADC_OBJECT( x ) ADC10_Adc0_##x
//% This is required for the ISR, since there's no way to include that in the Config class.
#define ADC10_Adc0_VECTOR_LOCATION	ADC10_VECTOR
//% Pointer to the dynamic lwevent (the one that the ADC users sets)
extern lwevent *ADC_OBJECT(waiting_lwevent_p);
//% Actual lwevent (the one that the ISR posts).
extern lwevent ADC_OBJECT(helper_lwevent);

class ADC10_Adc0_Config {
public:
	typedef uint16_t adc_result_type;
	const uint16_t CTL0 = SREF_1 | ADC10SHT_3 | ADC10SR | REFBURST | REF2_5V | REFON | ADC10ON;
	const uint16_t CTL1 = ADC10DIV_7 | ADC10SSEL_3;
	lwevent **const waiting_lwevent_p = &ADC_OBJECT(waiting_lwevent_p);
};

#undef ADC_OBJECT

#endif /* ADC10_ADC0_CONFIG_H_ */
