#include "SDLogDriver.h"

#include "Constants.h"

#include <SD.h>

__LOG_DATA log_data_pack;

File f;

char* const data_begin = (char*)(&log_data_pack); // Yes, yes, I know
int data_length = sizeof(log_data_pack);

void initLogger()
{
	f = SD.open(LOG_FILENAME, FILE_WRITE);
	f.write(BEGIN_LIFE_BYTE);
	f.flush();
}

void logDataPack()
{
	f.write(BEGIN_DATA_BYTE);
	f.write(data_begin, data_length);
	f.flush();
}

void dumpLog()
{
	f.close();

	f = SD.open(LOG_FILENAME, FILE_READ);

	while(f.peek() >= 0)
	{
		char c = f.read();
		if(c == BEGIN_LIFE_BYTE)
		{
			Serial.println("=== UNIT BOOTUP ===");
		}
		else if(c == BEGIN_DATA_BYTE)
		{
			for(int i = 0; i < data_length; ++i)
				data_begin[i] = f.read();

			Serial.print(log_data_pack.timestamp); Serial.print("ms: ");

			Serial.print("Acc ("); // g
			Serial.print(log_data_pack.acc_x); Serial.print(", ");
			Serial.print(log_data_pack.acc_y); Serial.print(", ");
			Serial.print(log_data_pack.acc_z); Serial.print(")\t");

			Serial.print("Gyro ("); // m / s^2
			Serial.print(log_data_pack.gyro_x); Serial.print(", ");
			Serial.print(log_data_pack.gyro_y); Serial.print(", ");
			Serial.print(log_data_pack.gyro_z); Serial.print(")\t");

			Serial.print("Ahrs ("); // degrees
			Serial.print(log_data_pack.ahrs_x * 180 / M_PI); Serial.print(", ");
			Serial.print(log_data_pack.ahrs_y * 180 / M_PI); Serial.print(", ");
			Serial.print(log_data_pack.ahrs_z * 180 / M_PI); Serial.print(")\t");

			Serial.print("Encoders ("); // mm / s
			Serial.print(log_data_pack.left_speed); Serial.print(", ");
			Serial.print(log_data_pack.right_speed); Serial.print(")\n");
		}
		else
		{
			Serial.println("Log file is corrupt");
			break;
		}
	}

	Serial.println("=== DUMP COMPLETE ===");

	f.close();

	SD.remove(LOG_FILENAME);

	initLogger();
}

void dropLog()
{
	f.close();

	Serial.println("=== LOG ERASED ===");

	SD.remove(LOG_FILENAME);

	initLogger();
}
