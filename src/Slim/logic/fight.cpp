#include "fight.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

#define STANDARD_SPEED				       1000

enum {
	SEARCH,
	FOUND,
	STOP_MOVING
} STATE = SEARCH;

void initFight()
{
	enqueueTurn(STANDARD_SPEED, 0, -20);
	enqueueLinearMovement(STANDARD_SPEED, 150);
	enqueueTurn(STANDARD_SPEED, 150, -360);
}

void handleFight()
{
	uint8_t left = readLeftSensor();
	uint8_t right = readRightSensor();

	if(STATE != FOUND)
	{
		if(left)
		{
			STATE = FOUND;
			clearQueue();
			enqueueTurn(STANDARD_SPEED, 0, -90);
		}
		else if(right)
		{
			STATE = FOUND;
			clearQueue();
			enqueueTurn(STANDARD_SPEED, 0, 90);
		}
	}
	else if(!(left || right))
	{
		enqueueLinearMovement(STANDARD_SPEED, 1000);
	}
}
