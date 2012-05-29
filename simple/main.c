#include "hax.h"
#include "compilers.h"

#define map_oi(oi, motor) \
	motor_set(IX_MOTOR(motor), oi_group_get(IX_OI_GROUP(1, oi)))
void telop_loop(void)
{
	index_t i = IX_OI_GROUP(1, 1);
	int8_t val = oi_group_get(i);

	index_t m = IX_MOTOR(1);
	puts("\tMOTOR");
	motor_set(m, val);

	puts("\tTELOP");
}

__noreturn void main(void)
{
	static ctrl_mode_t mode;

	arch_init_1();
	arch_init_2();

	for(;;) {
		mode = ctrl_mode_get();

		/* The "slow loop", executes once every SLOW_US microseconds. */
		if (do_slow_loop()) {
			arch_loop_1();
			puts("LOOP");
			printf("mode: %d\n", mode);
			switch (mode) {
			case MODE_AUTON:
				break;
			case MODE_TELOP:
				telop_loop();
				break;
			default:
				break;
			}

			puts("DONE1");
			arch_loop_2();
			puts("DONE2");
		}

		/* Executes as fast as the hardware allows. */
		arch_spin();
	}
}
