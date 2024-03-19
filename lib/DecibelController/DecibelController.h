#ifndef __DecibelController_H
#define __DecibelController_H
#include <Arduino.h>
#include "RecordController.h"

#define REFERENCE_SOUND_PRESSURE 0.023 //356e-6
// 0.0073700 = ~70dB

class DecibelController
{
public:
  static float calculateDecibel(SAMPLE_T *sampleBuffer, float sound_pressure_refference = REFERENCE_SOUND_PRESSURE);
};

#endif