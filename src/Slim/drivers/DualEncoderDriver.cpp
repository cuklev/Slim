#include "DualEncoderDriver.hpp"

#include <Arduino.h>

#define RESET_TIMEOUT_MULTIPLIER 3

DualEncoder::DualEncoder(int a_pin, int b_pin) :
	last_micros(0),
	impulse_counter(0),
	impulse_deltas_sum(0),
	impulse_deltas_index(0),
	speed(0),
	A_PIN(a_pin), B_PIN(b_pin) {
		for(auto& x : impulse_deltas) x = 0;
	}

void DualEncoder::A_handler() {
	update(digitalRead(A_PIN) == digitalRead(B_PIN));
}

void DualEncoder::B_handler() {
	update(digitalRead(A_PIN) != digitalRead(B_PIN));
}

void DualEncoder::init(void (*a_handler)(), void (*b_handler)())
{
	pinMode(A_PIN, INPUT);
	pinMode(B_PIN, INPUT);

	attachInterrupt(digitalPinToInterrupt(A_PIN), a_handler, CHANGE);
	attachInterrupt(digitalPinToInterrupt(B_PIN), b_handler, CHANGE);
}

void DualEncoder::update(int8_t dir)
{
	uint32_t current_micros = micros();

	dir = dir * 2 - 1; // 1 is forward
	if(impulse_counter == 0 || (dir > 0 && impulse_counter < 0) || (dir < 0 && impulse_counter > 0))
	{
		impulse_counter = dir;
		impulse_deltas_sum = 0;
		speed = 0;
	}
	else
	{
		if(impulse_deltas_index + 1 == IMPULSES_PER_ROUND)
			impulse_deltas_index = 0;
		else ++impulse_deltas_index;

		if(dir > 0 && impulse_counter <= IMPULSES_PER_ROUND)
			++impulse_counter;
		else if(dir < 0 && impulse_counter >= -IMPULSES_PER_ROUND)
			--impulse_counter;
		else impulse_deltas_sum -= impulse_deltas[impulse_deltas_index];

		impulse_deltas[impulse_deltas_index] = current_micros - last_micros;
		impulse_deltas_sum += impulse_deltas[impulse_deltas_index];

		speed = DISTANCE_PER_IMPULSE * (impulse_counter - 1) / impulse_deltas_sum;
	}

	last_micros = current_micros;
}

void DualEncoder::update()
{
	uint32_t current_micros = micros();

	if(current_micros - last_micros > impulse_deltas[impulse_deltas_index] * RESET_TIMEOUT_MULTIPLIER)
	{
		speed = 0;
		impulse_counter = 0;
	}
}
