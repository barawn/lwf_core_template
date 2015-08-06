/*
 * usci_twi0.cpp
 *
 *  Created on: Aug 3, 2015
 *      Author: barawn
 */
#define COMPONENTS_DEFAULT
// Add components.h. If we're added, we'll know.
// Otherwise, we don't add anything, and the cpp files do nothing.
#include "../../components.h"
// Change these lines for a new TWI. Everything else autorenames.
#ifdef USCI_TWI0_H_
#define I2C_OBJECT( x ) USCI_Twi0_##x
typedef USCI_Twi0 This_TWI;

/////////////////////////////
// TWI objects.
/////////////////////////////
#pragma NOINIT
uint8_t I2C_OBJECT(isr_count);
#pragma NOINIT
uint8_t *I2C_OBJECT(isr_data_ptr);
#pragma NOINIT
i2c_transaction *This_TWI::transaction;
bool This_TWI::lock = false;
lwevent_store_fifo This_TWI::wait_queue;

lwevent I2C_OBJECT(state_lwevent) ((lwevent_handler_t) This_TWI::i2c_lwevent_handler, (lwevent *) lwevent::LWEVENT_WAITING);

#endif
