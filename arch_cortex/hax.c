#include <hax.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#include "stm32f10x.h"
#include "vex_hw.h"

#if defined(USE_STDPERIPH_DRIVER)
#include "stm32f10x_adc.h"
#include "stm32f10x_dma.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_misc.h"
#include "core_cm3.h"
#endif

#include "compilers.h"
#include "init.h"
#include "rcc.h"
#include "usart.h"
#include "spi.h"
#include "exti.h"

spi_packet_vex m2u;
spi_packet_vex u2m;

/*
 * INTERNAL FUNCTIONS
 */
void hw_setup1(void) {
	rcc_init();
	gpio_init();
	usart_init();
	spi_init();
	nvic_init();
	tim1_init();
	adc_init();
	exti_init();
	
	memset(&u2m, 0, sizeof u2m);
	memset(&m2u, 0, sizeof m2u);

	spi_packet_init_u2m(&u2m);
	spi_packet_init_m2u(&m2u);
	
	printf("[ INIT DONE ]\n");
}

void hw_setup2(void) {	
	while(!is_master_ready());
}

void hw_spin(void) {
}

void hw_loop1(void) {
	vex_spi_xfer(&m2u, &u2m);
	spi_transfer_flag = false;
}

void hw_loop2(void) {
}

bool hw_ready(void) {
	return spi_transfer_flag;
}

/*
 * EXTERNAL API
 */
mode_t mode_get(void) {
	if (m2u.m2u.sys_flags.b.autonomus) {
		return MODE_AUTON;
	} else if (m2u.m2u.sys_flags.b.disable) {
		return MODE_DISABLE;
	} else {
		return MODE_TELOP;
	}
}

uint16_t analog_get(index_t id) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (id) {
        /* VEXNet Joystick */
        case PIN_OI(1): /* Right Stick, X */
		return joystick->axis_2;

	case PIN_OI(2): /* Right Stick, Y */
		return joystick->axis_1;

	case PIN_OI(4): /* Left Stick, X */
		return joystick->axis_4;

	case PIN_OI(3): /* Left Stick, Y */
		return joystick->axis_3;

        /* ADCs */
	case PIN_DIGITAL(1):
        case PIN_DIGITAL(2):
        case PIN_DIGITAL(3):
        case PIN_DIGITAL(4):
        case PIN_DIGITAL(5):
        case PIN_DIGITAL(6):
        case PIN_DIGITAL(7):
        case PIN_DIGITAL(8):
	 	return adc_buffer[index - 1] >> 2;

    	default:
		ERROR();
		return 0;
	}
}

bool digital_oi_get(index_t index) {
	struct oi_data *joystick = &m2u.m2u.joysticks[0].b;

	switch (index) {
	case 5: /* Left Buttons, Up */
		return joystick->g8_u;
	case 6: /* Left Buttons, Down */
		return joystick->g8_d;
	case 7: /* Left Buttons, Left */
		return joystick->g8_l;
	case 8: /* Left Buttons, Right */
		return joystick->g8_r;
	case 9: /* Right Buttons, Up */
		return joystick->g7_u;
	case 10: /* Right Buttons, Down */
		return joystick->g7_d;
	case 11: /* Right Buttons, Left */
		return joystick->g7_l;
	case 12: /* Right Buttons, Right */
		return joystick->g7_r;
	case 13: /* Left Trigger, Up */
		return joystick->g5_u;
	case 14: /* Left Trigger, Down */
		return joystick->g5_d;
	case 15: /* Right Trigger, Up */
		return joystick->g6_u;
	case 16: /* Right Trigger, Down */
		return joystick->g6_d;
	default:
		ERROR();
		return false;
	}
}

/*
 * MOTOR AND SERVO OUTPUTS
 */
void analog_set(index_t index, int8_t sp) {
	/* Two-wire motors. */
	if (index == 1 || index == 10) {
		/* TODO Two wire motor support. */
	}
	/* Three-wire servo or servomotor */
	else if (2 <= index && index <= 9) {
		uint8_t val;
		sp = (sp < 0 && sp != -128) ? sp - 1 : sp;
		val = sp + 128;

		u2m.u2m.motors[index] = val;
	} else {
		ERROR();
	}
}

/*
 * SERIAL IO
 */
void _putc(char c) {
	putchar(c);
}