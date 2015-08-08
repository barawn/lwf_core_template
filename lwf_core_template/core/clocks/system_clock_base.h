/*
 * system_clock_base.h
 *
 * Base class for a system clock. A system clockis a long-interval wait timer, that
 * allows functions to wait for multiple seconds. It can be implemented with different
 * hardware, although the most common hardware is using the watchdog timer, which also
 * adds a software watchdog to prevent something stupid happening.
 *
 * The system clock base uses 2 lwevent_store_fifos, one for the current time, and
 * one for the rollover time. Rollovers are detected when the new time (fetched in tick)
 * occurs before the *old* time. When that occurs, all of the system timers in the
 * current store are posted (yay for lwevent_store_fifo), the pointers are swapped
 * (so rollover->current, current->rollover), and then normal processing occurs.
 *
 * Note: delays are always +/- 1 tick.
 *
 * System clocks require a queue1. They probably require a queue0 too, for an ISR, but
 * hey, who knows.
 *
 *  Created on: Aug 4, 2015
 *      Author: barawn
 */

#ifndef SYSTEM_CLOCK_BASE_H_
#define SYSTEM_CLOCK_BASE_H_

#include <stdint.h>
#include "../lwevent.h"

extern lwevent_queue queue1;

//% \brief A system timer is just a lwevent, wrapped with a count around it which determines when it's going to be called.
//%
//% System timers are long-scale delays (ms-to-seconds) that allow functions to be periodically called.
//% They aren't really anything other than a lwevent with a 'count' variable tacked onto it.
class system_timer : public lwevent {
public:
	system_timer(lwevent_handler_t handler) : lwevent(handler, (lwevent *) lwevent::LWEVENT_WAITING) { }
	system_timer() {}
	uint16_t count;
};

template<class HW, class CONFIG>
class system_clock_base {
public:
	system_clock_base() {}

	void init() {
		HW::init_impl();
	}

	uint16_t time() {
		return *count;
	}

	// Adds the system timer at a delay of 'delay' from current time.
	void add(system_timer *p, uint16_t delay) {
		uint16_t cur_time;
		lwevent_store_fifo *store;
		system_timer *p2;
		system_timer *p3;

		cur_time = *count;
		if (HW::phase) store = &HW::store1;
		else store = &HW::store0;

		p->count = cur_time + delay;
		if (p->count < cur_time) {
			// it rolled over
			if (HW::phase) store = &HW::store0;
			else store = &HW::store1;
		}
		if (store->empty()) {
			store->store(p);
			return;
		}
		// Store is not empty.
		p2 = (system_timer *) store->get_first();
		if (p->count < p2->count) {
			// we're before, so we're the new head
			store->store_at_head(p);
			return;
		}
		while (p2->next != (lwevent *) lwevent::LWEVENT_TAIL) {
			p3 = (system_timer *) p2->next;
			// Are we after p2 and before p3?
			if (p->count < p3->count) {
				// Insert between.
				p2->next = p;
				p->next = p3;
				return;
			}
			// no, keep looking
			p2 = p3;
		}
		// We're at the end, so just add.
		store->store(p);
	}

	static void tick(lwevent *ev) {
		uint16_t tmp;
		lwevent_store_fifo *store;
		system_timer *p;
		lwevent *p2;
		ev->clear();

		// Fetch count. Count is incremented in the implementation, not here.
		// Typically it's done in the ISR, and then this event is just posted.
		// So this is now a temporary.
		tmp = *count;
		if (HW::phase) store = &HW::store1;
		else store = &HW::store0;
		if (tmp < HW::last_count) {
			queue1.post_store(*store);
			if (HW::phase) store = &HW::store0;
			else store = &HW::store1;
			HW::phase = !HW::phase;
		}
		HW::last_count = tmp;
		// Anything to do?
		if (store->empty()) return;
		// Yes.
		do {
			p = (system_timer *) store->get_first();
			// Timer expired?
			if (p->count <= tmp) {
				p2 = p->next;
				// Yes. Pluck it off.
				store->pop();
				// And post it (without check).
				// Note that we *post* here, we don't *call* it, because we want to do this out of queue0,
				// and the order in which system timers are called isn't guaranteed.

				// Other store types might handle them all themselves, because the order is important.
				// We post without check because it *did* have a next (it was in a store) and we want it
				// overwritten.
				queue1.post_without_check(p);
			} else break;
		} while (p2 != (lwevent *) lwevent::LWEVENT_TAIL);
	}

	volatile uint16_t *const count = CONFIG::count;
	const int ticks_per_second = CONFIG::ticks_per_second;
};



#endif /* SYSTEM_CLOCK_BASE_H_ */
