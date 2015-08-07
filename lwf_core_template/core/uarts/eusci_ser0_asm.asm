	.define	0, EUSCI_SER0_H_
	.cdecls CPP, LIST, "components.h"
GlobalIf:	.if	EUSCI_SER0_H_ = 1
	.include "../lwevent_asm.asm"

EUSCI_SER0_ISR:
	; Jump table.
	add.w	&(UCA0IV+EUSCI_Ser0_Config_enum.BASE-__MSP430_BASEADDRESS_EUSCI_A0__), PC
	; 0: No interrupt.
	reti
	; 2: RXIFG.
	jmp	EUSCI_SER0_RX_ISR
	; 4: TXIFG
	jmp	EUSCI_SER0_TX_ISR
	; 6: STTIFG
	reti
	; 8: TXCPTIFG
	reti
EUSCI_SER0_RX_ISR:
	push	r15
	mov.b	&EUSCI_Ser0_pointers.rx_wr, r15
	mov.b	&(UCA0RXBUF+EUSCI_Ser0_Config_enum.BASE-__MSP430_BASEADDRESS_EUSCI_A0__), EUSCI_Ser0_rx_buffer(r15)
	inc.b	r15
	bic.b	#EUSCI_Ser0_Config_enum.RX_BUF_SIZE, r15
	mov.b	r15, &EUSCI_Ser0_pointers.rx_wr
	pop		r15
	isr_lwevent_post	EUSCI_Ser0_rx_data_event
	bic.w	#(LPM3), 0(SP)
	reti
EUSCI_SER0_TX_ISR:
	; New byte to transmit?
	cmp.b	&EUSCI_Ser0_pointers.tx_wr, &EUSCI_Ser0_pointers.tx_rd
	; If equal, there isn't.
	jeq		EUSCI_Ser0_TX_ISR_empty
	; Not equal, so there is one. Copy it to TXBUF, increment pointer, and continue.
	push	r15
	mov.b	&EUSCI_Ser0_pointers.tx_rd, r15
	mov.b	EUSCI_Ser0_tx_buffer(r15), &(UCA0TXBUF+EUSCI_Ser0_Config_enum.BASE-__MSP430_BASEADDRESS_EUSCI_A0__)
	inc.b	r15
	bic.b	#EUSCI_Ser0_Config_enum.RX_BUF_SIZE, r15
	mov.b	r15, &EUSCI_Ser0_pointers.tx_rd
	pop		r15
	reti
EUSCI_Ser0_TX_ISR_empty:
	bic.b	#UCTXIE, &(UCA0IE+EUSCI_Ser0_Config_enum.BASE-__MSP430_BASEADDRESS_EUSCI_A0__)
	isr_lwevent_post_wakeup	EUSCI_Ser0_tx_empty_event, 0
	reti


	.sect 	EUSCI_Ser0_VECTOR_LOCATION
	.short	EUSCI_SER0_ISR
	.endif
