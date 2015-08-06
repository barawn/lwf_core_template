/*
 * eusci_ser0.h: Header file for using EUSCI A0 as a UART.
 *
 *  Created on: Jul 1, 2015
 *      Author: barawn
 */

#ifndef EUSCI_SER0_H_
#define EUSCI_SER0_H_

#include "eusci_uart.h"
#include "config/eusci_ser0_config.h"

typedef EUSCI_UART<EUSCI_Ser0_Config> EUSCI_Ser0;

#endif /* EUSCI_SER0_H_ */
