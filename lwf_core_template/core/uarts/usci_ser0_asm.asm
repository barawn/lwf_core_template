	; First define USCI_SER0_H_ as 0. Then include components.h, and we'll see if we're supposed to be here.
	.define 0, USCI_SER0_H_
	.cdecls CPP, LIST, "components.h"

	; This prevents this entire file from doing anything if the USCI_Ser0 isn't included.
GlobalIf:	.if	USCI_SER0_H_ = 1
	.include "../usci_magic.asm"
	usci_uart_magic USCI_Ser0_Config_enum.USCI_Ser0_INDEX
	.include "../lwevent_asm.asm"

	.global	USCI_Ser0_RX_ISR
	.global	USCI_Ser0_TX_ISR

USCI_Ser0_RX_ISR:
	push	r15
	mov.b	&USCI_Ser0_pointers.rx_wr, r15
	mov.b	&usci_uart_RXBUF, USCI_Ser0_rx_buffer(r15)
	inc.b	r15
	bic.b	#USCI_Ser0_Config_enum.USCI_Ser0_RX_BUF_SIZE, r15
	mov.b	r15, &USCI_Ser0_pointers.rx_wr
	pop		r15
	isr_lwevent_post	USCI_Ser0_rx_data_event
	bic.w	#(LPM3), 0(SP)
	reti
USCI_Ser0_TX_ISR:
	; New byte to transmit?
	cmp.b	&USCI_Ser0_pointers.tx_wr, &USCI_Ser0_pointers.tx_rd
	; If equal, there isn't.
	jeq		USCI_Ser0_TX_ISR_empty
	; Not equal, so there is one. Copy it to TXBUF, increment pointer, and continue.
	push	r15
	mov.b	&USCI_Ser0_pointers.tx_rd, r15
	mov.b	USCI_Ser0_tx_buffer(r15), &usci_uart_TXBUF
	inc.b	r15
	bic.b	#USCI_Ser0_Config_enum.USCI_Ser0_TX_BUF_SIZE, r15
	mov.b	r15, &USCI_Ser0_pointers.tx_rd
	pop		r15
	reti
USCI_Ser0_TX_ISR_empty:
	bic.b	#usci_uart_TXIE, &usci_uart_IE
	isr_lwevent_post_wakeup	USCI_Ser0_tx_empty_event, 0
	reti

USCI_Ser0_If0:	.if	USCI_Ser0_RX_VECTOR_LOCATION != 0
	.sect 	USCI_Ser0_RX_VECTOR_LOCATION
	.short	USCI_Ser0_RX_ISR
	.endif
USCI_Ser0_If1:	.if	USCI_Ser0_TX_VECTOR_LOCATION != 0
	.sect	USCI_Ser0_TX_VECTOR_LOCATION
	.short	USCI_Ser0_TX_ISR
	.endif



	.endif
