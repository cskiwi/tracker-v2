#include "LoggerController.h"

LoggerController::LoggerController()
{
}

void LoggerController::log(const int duration, const int decibelInterval, float gain)
{
  int logInterval = duration / 1000;

  // print that we are running a log cycle with the current time
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Running log cycle");

  // create a record controller and file controller
  RecordController recordController;
  FileController fileController;

  recordController.setGain(gain);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Controllers initialized");

  // open the decibel file
  String dbAfileName = "decibel.csv";
  file_t decibelFile = fileController.openDBFile(dbAfileName, true);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] " + dbAfileName + " opened");

  // recording us
  uint8_t recordingTimeUs = logInterval * 1000000;

  // adding rnd number to file name
  String fileName = "sound_" + TimeController::getFormattedFile() + ".wav";
  file_t soundFile = fileController.openSoundFile(fileName, true, recordingTimeUs);

  Serial.println("[" + TimeController::getFormattedDateTime() + "] " + fileName + " opened");

  int recordSize = recordController.getRecordSize(logInterval);
  int decibelSize = recordController.getRecordSize(decibelInterval);
  int totalRecorded = 0;
  int lastDecibel = 0;

  Serial.println("[" + TimeController::getFormattedDateTime() + "] Recording " + String(recordSize) + " bytes");

  while (totalRecorded < recordSize)
  {
    size_t bytes_read = 0;
    // record data sample
    SAMPLE_T *sample = recordController.recordSample(&bytes_read);

    // if sample is null, retry
    if (sample == NULL)
    {
      Serial.println("[" + TimeController::getFormattedDateTime() + "] Sample is null, retrying");
      // i--;
      continue;
    }

    totalRecorded += bytes_read;

    // write sample to file
    fileController.writeSample(soundFile, (const byte *)sample);

    if ((totalRecorded - lastDecibel) > decibelSize)
    {
      lastDecibel = totalRecorded;


      // calculate decibel
      float dba = DecibelController::calculateDecibel(sample);

      // write decibel to file
      String dbALine = TimeController::getFormattedDateTime() + ", " + String(dba);
      fileController.writeLine(decibelFile, dbALine.c_str());
      Serial.println("[" + TimeController::getFormattedDateTime() + "] Dba: " + String(dba) + " dB(A)");
    }

    // free sample memory
    free(sample);
  }

  // close the decibel file
  decibelFile.close();
  soundFile.close();

  // print that we are done with the log cycle
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Log cycle complete");
}
