	.define 0, USCI_TWI0_H_
	.cdecls CPP, LIST, "components.h"

GlobalIf:	.if	USCI_TWI0_H_ = 1
	.include "../usci_magic.asm"
	usci_i2c_magic USCI_Twi0_Config_enum.USCI_Twi0_INDEX
	.include "../lwevent_asm.asm"

	.global	USCI_Twi0_FLAG_ISR
	.global	USCI_Twi0_RXTX_ISR

	.asmfunc
USCI_Twi0_FLAG_ISR:
	; One of the "bad things happened" flags went off.
	bit.b	#UCNACKIFG, &usci_i2c_STAT
	jz		USCI_Twi0_FLAG_ISR_no_stop
	bis.b	#UCTXSTP, &usci_i2c_CTL1
USCI_Twi0_FLAG_ISR_no_stop:
	bic.b	#(usci_i2c_TXIE+usci_i2c_RXIE), &usci_i2c_IE
	clr.b	&usci_i2c_I2CIE
	; Because we self-disable, this lwevent can only post once.
	isr_lwevent_post_nocheck USCI_Twi0_state_lwevent
	bic		#(LPM3), 0(SP)
	reti
	.endasmfunc


	.asmfunc
USCI_Twi0_RXTX_ISR:
	bit.b	#usci_i2c_RXIFG, &usci_i2c_IFG
	jnz		USCI_Twi0_RX_ISR
	bit.b	#UCTXSTT, &usci_i2c_CTL1
	jnz		USCI_Twi0_TX_ISR_start_not_done
	dec.b	&USCI_Twi0_isr_count
	jz		USCI_Twi0_TX_ISR_done
	push	r15
	mov.w	&USCI_Twi0_isr_data_ptr, r15
	mov.b	@r15+, &usci_i2c_TXBUF
	mov.w	r15, &USCI_Twi0_isr_data_ptr
	pop		r15
	reti
USCI_Twi0_TX_ISR_start_not_done:
	push	r15
	mov.w	&USCI_Twi0_isr_data_ptr, r15
	mov.b	@r15, &usci_i2c_TXBUF
	pop		r15
	reti
USCI_Twi0_TX_ISR_done:
	bis.b	#UCTXSTP, &usci_i2c_CTL1
	bic.b	#usci_i2c_TXIE, &usci_i2c_IE
	clr.b	&usci_i2c_I2CIE
	; Because we self-disable, this lwevent can only post once.
	isr_lwevent_post_nocheck USCI_Twi0_state_lwevent
	bic		#(LPM3), 0(SP)
	reti
USCI_Twi0_RX_ISR:
	push	r15
	mov.w	&USCI_Twi0_isr_data_ptr, r15
	mov.w	&usci_i2c_RXBUF, 0(r15)
	inc.w	r15
	mov.w	r15, &USCI_Twi0_isr_data_ptr
	pop		r15
	dec.b	&USCI_Twi0_isr_count
	jc		USCI_Twi0_RX_ISR_done
	jnz		USCI_Twi0_RX_ISR_reti
	bis.b	#UCTXSTP, &usci_i2c_CTL1
USCI_Twi0_RX_ISR_reti:
	reti
USCI_Twi0_RX_ISR_done:
	bic.b	#usci_i2c_RXIE, &usci_i2c_IE
	clr.b	&usci_i2c_I2CIE
	; Because we self-disable, this lwevent can only post once.
	isr_lwevent_post_nocheck USCI_Twi0_state_lwevent
	bic		#(LPM3), 0(SP)
	reti
	.endasmfunc

USCI_Twi0_If0:	.if	USCI_Twi0_RXTX_VECTOR_LOCATION != 0
	.sect 	USCI_Twi0_RXTX_VECTOR_LOCATION
	.short	USCI_Twi0_RXTX_ISR
	.endif
USCI_Twi0_If1:	.if	USCI_Twi0_FLAG_VECTOR_LOCATION != 0
	.sect	USCI_Twi0_FLAG_VECTOR_LOCATION
	.short	USCI_Twi0_FLAG_ISR
	.endif

	.endif
