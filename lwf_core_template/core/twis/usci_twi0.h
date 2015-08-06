/*
 * usci_twi0.h
 *
 * Implementation of an I2C (two-wire interface) master on USCI B0. It gets the global name
 * USCI_Twi0 (and the corresponding global config object is USCI_Twi0_Config).
 *
 *  Created on: Aug 3, 2015
 *      Author: barawn
 */

#ifndef USCI_TWI0_H_
#define USCI_TWI0_H_

#include "usci_i2c.h"
#include "config/usci_twi0_config.h"

typedef USCI_I2C<USCI_Twi0_Config> USCI_Twi0;

#endif /* USCI_TWI0_H_ */
