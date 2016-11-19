#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include "OrientationSensorsWrapper.hpp"
#include "DualEncoderDriver.hpp"
#include "utility/Point3D.hpp"

void initLogger();
void logDataPack(OrientationSensors&, DualEncoder&, DualEncoder&, const Point3D<double>&, const Point3D<double>&);
void dumpLog();
void dropLog();

#endif // __SD_LOG_DRIVER_H