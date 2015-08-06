/*
 * eusci_ser0_asm.h
 *
 *  Created on: Jul 1, 2015
 *      Author: barawn
 */

#ifndef EUSCI_SER0_ASM_H_
#define EUSCI_SER0_ASM_H_

#include <stdint.h>
#include "eusci_ser0.h"

//% \brief Assembly convenience enum for EUSCI Ser0.
//%
//% The MSP430 assembler's .cdecl directive isn't
//% great at handling C++ code - if you put these
//% as constants, it just skips over them. But!
//% If you define them as enums, then magically,
//% it supports them. So we just port over all of
//% the important constants.
//%
//% Note that if we included this in the C++ class,
//% then the symbols would be mangled, and we would
//% have to extract their actual names. Leaving them
//% outside like this eliminates the mangling,
//% and then we know what they look like.


#endif /* EUSCI_SER0_ASM_H_ */
