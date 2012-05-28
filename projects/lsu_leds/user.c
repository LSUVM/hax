#include <hax.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "user.h"

/* Called as soon as the processor is initialized. Generally used to initialize
 * digital inputs, analog inputs, digital outputs, and interrupts.
 */
void init(void) {
	printf("INIT\n");

	digital_setup(IX_DIGITAL(1), DIGITAL_OUT); // White
	digital_setup(IX_DIGITAL(3), DIGITAL_OUT); // Green
	digital_setup(IX_DIGITAL(5), DIGITAL_OUT); // Yellow
	digital_setup(IX_DIGITAL(7), DIGITAL_OUT); // Red

    digital_set(IX_DIGITAL(1), true);
    digital_set(IX_DIGITAL(3), true);
    digital_set(IX_DIGITAL(5), true);
    digital_set(IX_DIGITAL(7), true);
}

/* Called every time the user processor receives data from the master processor
 * and is in autonomous mode.
 */
void auton_loop(void) {
}

/* Same as auton_loop(), except in user controlled mode. This is controlled by
 * the competition switch (Cortex) or input from the transmitter (PIC).
 */
void telop_loop(void) {}

/* Same as auton_loop() and telop_loop(), except in disabled mode. This is 
 * executed only when the transmitter is off and motor outputs are disabled.
 */
void disable_loop(void) {}

/* These functions share the same behavior as their _loop() counterparts, but
 * execute as quickly as the processor allows. Since inputs and outputs cannot
 * occur this quickly, the _loop() functions should be preferred.
 */
void auton_spin(void) {}
void telop_spin(void) {}
void disable_spin(void) {}

