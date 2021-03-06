#include "drivers/DualEncoderDriver.hpp"
#include "drivers/SDLogDriver.hpp"
#include "drivers/MotorDriver.h"
#include "drivers/ProximitySensors.h"
#include "modules/OrientationSensorsWrapper.hpp"
#include "planar/PlanarAccelerationModule.hpp"
#include "planar/PlanarSpeedModule.hpp"
#include "control/RobotStateControl.h"
#include "control/SerialCommander.h"
#include "control/Movement.h"

#include <Wire.h>
#include <SD.h>

#define SERIAL_BAUD_RATE        115200
#define I2C_FREQUENCY           400000
#define SD_CHIP_SELECT          4

#define LOG_FREQUENCY           50

#define SAMPLE_FREQUENCY        208

#define LEFT_A_PIN              12
#define LEFT_B_PIN              11
#define RIGHT_A_PIN             9
#define RIGHT_B_PIN             10

DualEncoder leftEncoder(LEFT_A_PIN, LEFT_B_PIN);
DualEncoder rightEncoder(RIGHT_A_PIN, RIGHT_B_PIN);

OrientationSensors position;

void setup() {
	Serial.begin(SERIAL_BAUD_RATE);

	Wire.begin();
	Wire.setClock(I2C_FREQUENCY);

	SD.begin(SD_CHIP_SELECT);

	initLogger();

	leftEncoder.init([](){leftEncoder.A_handler();}, [](){leftEncoder.B_handler();});
	rightEncoder.init([](){rightEncoder.A_handler();}, [](){rightEncoder.B_handler();});

	initMotors();

	initProximitySensors();

	position.init();
}

void loop() {
	static uint32_t last_sample_micros = 0;
	static uint32_t last_log_micros = 0;

	uint32_t current_micros = micros();

	// Read sensors
	if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY) {
		position.update();
		leftEncoder.update();
		rightEncoder.update();
		updatePlanarSpeed(position);

		handleRobotAction(current_micros, []() {
			position.calibrate();
			initPlanarSpeed();
		});

		handleControlledMovement(leftEncoder.getSpeed(), rightEncoder.getSpeed(), (float)(current_micros - last_sample_micros) / MICROS_PER_SECOND);

		last_sample_micros = current_micros;
	}

	indicateRobotState(current_micros);

	// Log data
	if(current_micros - last_log_micros >= MICROS_PER_SECOND / LOG_FREQUENCY)
	{
		logDataPack(position, leftEncoder, rightEncoder);
		last_log_micros = current_micros;
	}

	getSerialCommand();
}
