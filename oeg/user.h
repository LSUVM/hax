#ifndef USER_H_
#define USER_H_

#if defined(ARCH_CORTEX)
enum {
	MOTOR_DRIVE_R1 = IX_MOTOR(2),
	MOTOR_DRIVE_R2 = IX_MOTOR(3),
	MOTOR_DRIVE_L1 = IX_MOTOR(4),
	MOTOR_DRIVE_L2 = IX_MOTOR(5),
	MOTOR_ARM_R1   = IX_MOTOR(6),
	MOTOR_ARM_R2   = IX_MOTOR(7),
	MOTOR_ARM_L1   = IX_MOTOR(8),
	MOTOR_ARM_L2   = IX_MOTOR(9)
};
#else
#error "unsupported architecture"
#endif

/* Our personal functions for this program */

void init(void);
void auton_loop(void);
void auton_spin(void);
void telop_loop(void);
void telop_spin(void);
void disable_loop(void);
void disable_spin(void);

#endif
