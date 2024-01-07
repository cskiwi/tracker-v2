#ifndef __DecibelController_H
#define __DecibelController_H
#include <Arduino.h>
#include "RecordController.h"

#define REFERENCE_SOUND_PRESSURE 356e-6

class DecibelController
{
public:
  static float calculateDecibel(SAMPLE_T *sampleBuffer);
};

#endif