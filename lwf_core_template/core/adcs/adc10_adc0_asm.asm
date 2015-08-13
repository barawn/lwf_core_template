	.define 0, ADC10_ADC0_H_
	.cdecls CPP, LIST, "components.h"

GlobalIf:	.if	ADC10_ADC0_H_ = 1
	.include "../lwevent_asm.asm"

	.asmfunc
ADC10_ISR:
	bic.b	#ADC10IE, ADC10CTL0;
	isr_lwevent_post_nocheck ADC10_Adc0_helper_lwevent
	bic		#(LPM3), 0(SP)
	reti
	.endasmfunc

ADC10_Adc0_If0:	.if	ADC10_Adc0_VECTOR_LOCATION != 0
	.sect 	ADC10_Adc0_VECTOR_LOCATION
	.short	ADC10_ISR
	.endif

	.endif
