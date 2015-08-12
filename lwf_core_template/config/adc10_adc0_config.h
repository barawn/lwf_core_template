/*
 * adc10_adc0_config.h
 *
 *  Created on: Aug 12, 2015
 *      Author: barawn
 */

#ifndef ADC10_ADC0_CONFIG_H_
#define ADC10_ADC0_CONFIG_H_

#define ADC_OBJECT( x ) ADC10_Adc0_##x
//% \brief This is required for the ISR, since there's no way to include that in the Config class.
#define ADC10_Adc0_VECTOR_LOCATION	ADC10_VECTOR
//% Actual pointer to the lwevent.
extern lwevent ADC_OBJECT(waiting_lwevent);

class ADC10_Adc0_Config {
public:
	const uint16_t CTL0 = SREF_1 | ADC10SHT_3 | ADC10SR | REFBURST | REF2_5V | REFON | ADC10ON;
	const uint16_t CTL1 = ADC10DIV_7 | ADC10SSEL_3;
	lwevent *const waiting_lwevent = &ADC_OBJECT(waiting_lwevent);
};

#undef ADC_OBJECT

#endif /* ADC10_ADC0_CONFIG_H_ */
