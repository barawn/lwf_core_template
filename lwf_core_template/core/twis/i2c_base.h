/*
 * i2c_base.h
 *
 * calling order:
 * if (twi0.available()) {
 *    twi0.prepare(&my_txn);
 *    twi0.submit();
 * } else {
 *    twi0.wait_for_available(&my_lwevent);
 * }
 *
 * When my_txn is complete, it will call the handler:
 * my_txn_handler(lwevent *ev) {
 *    ev->clear();
 *    // check status, do something
 *    if (done) twi0.release();
 *    else {
 *       my_txn.address = next_address;
 *       twi0.submit();
 *    }
 * }
 *
 * So prepare only needs to be called at the beginning
 * of the chain - after that you can just call submit, submit, submit
 * until you're ready to give it up, at which point you call release().
 *
 * Prepare CAN be called in the middle of an I2C sequence if you don't
 * want to keep reusing the same one. Canned I2C sequences could do this, for instance.
 *
 *  Created on: Jul 24, 2015
 *      Author: barawn
 */

#ifndef I2C_BASE_H_
#define I2C_BASE_H_

#include <stdint.h>
#include "../lwevent.h"

typedef enum i2c_dirtype_t {
	I2C_DIRECTION_WRITE = 0x00,			//< Just for clarity.
	I2C_DIRECTION_READ = 0x01,			//< Set if it's a read, otherwise it's a write.
} i2c_dirtype_t;

//% \brief Base class for a *simple* I2C transaction.
//%
//% Like most things in the lwevent framework, the I2C transaction is just a
//% promoted lwevent: in this case with a data pointer and status call.
//%
//% Most TWI interfaces will use this as their I2C transaction type, but
//% the config file can redefine another type (which obviously needs to have
//% lwevent as its most derived class) which must contain status, and data
//% as members. (This could be used to make an i2c_timer_transaction
//% which derives from system_timer instead).
class i2c_transaction : public lwevent {
public:
	i2c_transaction(uint8_t *initData, lwevent_handler_t initHandler) :
		data(initData),
		lwevent(initHandler, (lwevent *) lwevent::LWEVENT_WAITING) { }
	i2c_transaction() {}
	uint8_t status;
	uint8_t *data;
};

template<class T, class CONFIG>
class i2c_base {
public:
	typedef typename CONFIG::i2c_transaction_type transaction_type;
	const uint8_t I2C_STATUS_OK = 0;
	const uint8_t I2C_STATUS_BUSY = 1;
	const uint8_t I2C_STATUS_POLLING_START = 2;
	const uint8_t I2C_STATUS_NACK = 4;
	const uint8_t I2C_STATUS_ARBITRATION_LOST = 8;
	const uint8_t I2C_STATUS_FLAG_REPEATED_START = 0x80; //< might not be supported

	i2c_base() {}
	static void init() {
		T::transaction = (transaction_type *) lwevent::LWEVENT_WAITING;
		T::init_impl();
	}
	static void prepare(i2c_transaction *t) {
		if (T::transaction != (transaction_type *) lwevent::LWEVENT_WAITING) return;
		T::transaction = t;
	}
	static void submit(uint8_t address, uint8_t len, i2c_dirtype_t dirtype) {
		T::submit_impl(address, len, dirtype);
	}
	static void complete() {
		transaction_type *tmp;
		if (T::transaction == (transaction_type *) lwevent::LWEVENT_WAITING) return;
		tmp = T::transaction;
		tmp->handler(tmp);
	}
	static bool available() {
		return (T::transaction == (transaction_type *) lwevent::LWEVENT_WAITING);
	}
	static void release() {
		lwevent *t;
		T::transaction = (transaction_type *) lwevent::LWEVENT_WAITING;
		if (T::wait_queue.empty()) return;
		t = T::wait_queue.pop();
		t->handler(t);
	}
	static void wait_for_available(lwevent *t) {
		if (T::transaction == (transaction_type *) lwevent::LWEVENT_WAITING) {
			t->handler(t);
		} else {
			T::wait_queue.store(t);
		}
	}
};

#endif /* I2C_BASE_H_ */
