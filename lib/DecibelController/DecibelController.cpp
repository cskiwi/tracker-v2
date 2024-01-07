#include "DecibelController.h"


float DecibelController::calculateDecibel(SAMPLE_T *sampleBuffer)
{
  // get sample size
  int sample_size = sizeof(sampleBuffer) / sizeof(sampleBuffer[0]);
  
  // get highest peak
  float highest_peak = 0;
  for (int i = 0; i < sample_size; i += 2)
  {
    int16_t sample = (int16_t)((sampleBuffer[i + 1] << 8) | sampleBuffer[i]);
    float sample_abs = abs(sample);
    if (sample_abs > highest_peak)
    {
      highest_peak = sample_abs;
    }
  }

  // calculate dbA
  float rms = sqrt(highest_peak / (sample_size / 2));
  float dbA = 20 * log10(rms / REFERENCE_SOUND_PRESSURE);

  return dbA;
}
