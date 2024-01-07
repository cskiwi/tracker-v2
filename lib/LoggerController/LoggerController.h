#ifndef __LoggerController_H
#define __LoggerController_H
#include <Arduino.h>

#include "RecordController.h"
#include "FileController.h"
#include "TimeController.h"
#include "DecibelController.h"

class LoggerController
{
public:
  LoggerController();

  void log();
};

#endif