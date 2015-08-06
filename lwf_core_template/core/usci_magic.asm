;
; Stupid macro to allow finding the registers for a given USCI instance.
; This is sadly necessary because for some MSP430s, they don't define
; BASEADDRESS for modules in the header file, so the .cdecls parser can't
; figure out the offsets.
;
usci_i2c_magic 	.macro		THIS_INDEX
If?:	.if THIS_INDEX = 0
		.define		UCB0RXBUF,	usci_i2c_RXBUF
		.define		UCB0TXBUF,	usci_i2c_TXBUF
		.define		IE2,		usci_i2c_IE
		.define		IFG2,		usci_i2c_IFG
		.define		UCB0TXIFG,	usci_i2c_TXIFG
		.define		UCB0RXIFG,	usci_i2c_RXIFG
		.define		UCB0I2CIE,	usci_i2c_I2CIE
		.define		UCB0RXIE,	usci_i2c_RXIE
		.define		UCB0TXIE,	usci_i2c_TXIE
		.define		UCB0CTL1,	usci_i2c_CTL1
		.define		UCB0STAT,	usci_i2c_STAT
		.elseif	THIS_INDEX = 1
		.define		UCB1RXBUF,	usci_i2c_RXBUF
		.define		UCB1TXBUF,	usci_i2c_TXBUF
		.define		UC1IE,		usci_i2c_IE
		.define		UC1IFG,		usci_i2c_IFG
		.define		UCB1TXIFG,	usci_i2c_TXIFG
		.define		UCB1RXIFG,	usci_i2c_RXIFG
		.define		UCB1I2CIE,	usci_i2c_I2CIE
		.define		UCB1RXIE,	usci_i2c_RXIE
		.define		UCB1TXIE,	usci_i2c_TXIE
		.define		UCB1CTL1,	usci_i2c_CTL1
		.define		UCB1STAT,	usci_i2c_STAT
		.endif
		.endm

usci_uart_magic	.macro		THIS_INDEX
If?:	.if	THIS_INDEX = 0
		.define		UCA0RXBUF,	usci_uart_RXBUF
		.define		UCA0TXBUF,	usci_uart_TXBUF
		.define		IE2,		usci_uart_IE
		.define		UCA0TXIE,	usci_uart_TXIE
		.elseif THIS_INDEX = 1
		.define		UCA1RXBUF,	usci_uart_RXBUF
		.define		UCA1TXBUF,	usci_uart_TXBUF
		.define		UC1IE,		usci_uart_IE
		.define		UCA1TXIE,	usci_uart_TXIE
		.endif
		.endm

