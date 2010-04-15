#include <hax.h>
#include <stdio.h>

#include "encoder.h"
#include "ports.h"
#include "util.h"

#include "robot.h"

#if defined(ROBOT_KEVIN)

void drive_raw(AnalogOut left, AnalogOut right) {
	motor_set(MTR_DRIVE_L, +left);
	motor_set(MTR_DRIVE_R, -right);
}

void arm_raw(AnalogOut vel) {
	motor_set(MTR_ARM_A1, +vel);
	motor_set(MTR_ARM_A2, +vel);
	motor_set(MTR_ARM_B1, -vel);
	motor_set(MTR_ARM_B2, -vel);
}

void ramp_raw(AnalogOut left, AnalogOut right) {
	motor_set(MTR_LIFT_L, -left);
	motor_set(MTR_LIFT_R, +right);
}

bool ramp_smart(AnalogOut vel) {
	int16_t pos  = analog_adc_get(POT_LIFT);
	bool    up   = vel > 0 && LIFT_LT(pos, POT_LIFT_HIGH);
	bool    down = vel < 0 && LIFT_GT(pos, POT_LIFT_LOW);
	bool    move = up || down;

	ramp_raw(move * vel, move * vel);

	return !move;
}

#elif defined(ROBOT_NITISH)

void drive_raw(AnalogOut left, AnalogOut right) {
	motor_set(MTR_DRIVE_L1, +left);
	motor_set(MTR_DRIVE_L2, +left);
	motor_set(MTR_DRIVE_R1, -right);
	motor_set(MTR_DRIVE_R2, -right);
}

void arm_raw(AnalogOut vel) {
	motor_set(MTR_ARM_L, +vel);
	motor_set(MTR_ARM_R, -vel);
}

void ramp_raw(AnalogOut left, AnalogOut right) {
	motor_set(MTR_LIFT_L, left);
	motor_set(MTR_LIFT_R, right);
}

bool ramp_smart(AnalogOut vel) {
	int16_t left  = analog_adc_get(POT_LIFT_L);
	int16_t right = analog_adc_get(POT_LIFT_R);

	bool move_left  = (vel > 0 && LIFT_L_LT(left, POT_LIFT_L_HIGH))
	               || (vel < 0 && LIFT_L_GT(left, POT_LIFT_L_LOW));
	bool move_right = (vel > 0 && LIFT_R_LT(right, POT_LIFT_R_HIGH))
	               || (vel < 0 && LIFT_R_GT(right, POT_LIFT_R_LOW));

	ramp_raw(move_left * vel, move_right * vel);

	return !(move_left || move_right);
}

#endif

void drive_smart(AnalogOut forward, AnalogOut turn) {
	int16_t left  = (int16_t) forward - turn;
	int16_t right = (int16_t) forward + turn;
	int16_t max   = MAX(ABS(left), ABS(right));

	/* Scale the values to not exceed kMotorMax. */
	if (max > kMotorMax) {
		left  = left  * kMotorMax / max;
		right = right * kMotorMax / max;
	}

	drive_raw(-left, right);
}

bool arm_smart(AnalogOut vel) {
	int16_t pos  = analog_adc_get(POT_ARM);
	bool    up   = vel > 0 && ARM_LT(pos, POT_ARM_HIGH);
	bool    down = vel < 0 && ARM_GT(pos, POT_ARM_LOW);
	bool    move = up || down;

	arm_raw(move * vel);

	return !move;
}

int32_t drive_straight(AnalogOut forward) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t diff  = left - right;
	int32_t turn  = SIGN(diff) * PROP(ABS(forward), DRIVE_STRAIGHT_ERRMAX, ABS(diff));

	drive_smart(forward, turn);

	return (left + right) / 2;
}

bool drive_turn(int16_t angle) {
	int32_t left  = encoder_get(ENC_L);
	int32_t right = encoder_get(ENC_R);
	int32_t cur   = ABS(left - right) / 2;
	int32_t tar   = (int32_t)ABS(angle) * ENC_PER_DEG;
	int32_t diff  = ABS(cur - tar);

	int32_t tmp1  = (int32_t)kMotorMax * diff / DRIVE_TURN_ERRMAX;
	int32_t turn  = PROP(kMotorMax, DRIVE_TURN_ERRMAX, diff);

	drive_smart(0, SIGN(angle) * turn);

	return ABS(cur - tar) <= DRIVE_TURN_ERRMIN;
}
