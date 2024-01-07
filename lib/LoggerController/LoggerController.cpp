#include "LoggerController.h"

LoggerController::LoggerController()
{
}

void LoggerController::log()
{
  uint8_t logInterval = 10;

  // print that we are running a log cycle with the current time
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Running log cycle");

  // create a record controller and file controller
  RecordController recordController;
  FileController fileController;

  // open the decibel file
  file_t decibelFile = fileController.openDBFile("test.txt", true);

  // recording us
  uint8_t recordingTimeUs = logInterval * 1000000;
  file_t soundFile = fileController.openSoundFile("test.wav", true, recordingTimeUs);

  for (int i = 0; i < logInterval; i++)
  {
    // record data sample
    SAMPLE_T *sample = recordController.recordSample();

    // write sample to file
    fileController.writeSample(soundFile, (const byte *)sample);

    // calculate decibel
    float dba = DecibelController::calculateDecibel(sample);

    // write decibel to file
    String dbALine = TimeController::getFormattedDateTime() + ", " + String(dba);
    fileController.writeLine(decibelFile, dbALine.c_str());
    Serial.println("[" + TimeController::getFormattedDateTime() + "] Dba: " + String(dba) + " dB(A)");

    // free sample memory
    free(sample);
  }

  // close the decibel file
  decibelFile.close();
  soundFile.close();

  // print that we are done with the log cycle
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Log cycle complete");
}
