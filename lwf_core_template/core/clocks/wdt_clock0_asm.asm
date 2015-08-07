	.define 0, WDT_CLOCK0_H_
	.cdecls CPP, LIST, "../../components.h"

GlobalIf: .if WDT_CLOCK0_H_ = 1
	.include "../lwevent_asm.asm"

WDT_Clock0_ISR:
	inc.w	&WDT_Clock0_count
	isr_lwevent_post_wakeup WDT_Clock0_tick_lwevent, 0
	reti

	.sect WDT_Clock0_VECTOR_LOCATION
	.short WDT_Clock0_ISR
	.endif
