#ifndef __FileController_H
#define __FileController_H
#include <Arduino.h>

#include "RecordController.h"
#include "TimeController.h"
#include "RecordController.h"

#include "FS.h"
#include "SPI.h"
#include <SdFat.h>
#include "Wire.h"

#define SD_FAT_TYPE 2

// SDCARD_SS_PIN is defined for the built-in SD on some boards.
#ifndef SDCARD_SS_PIN
const uint8_t SD_CS_PIN = GPIO_NUM_5;
#else  // SDCARD_SS_PIN
// Assume built-in SD is used.
const uint8_t SD_CS_PIN = SDCARD_SS_PIN;
SDCARD_SS_PIN
#endif //

// Try max SPI clock for an SD. Reduce SPI_CLOCK if errors occur.
#define SPI_CLOCK SD_SCK_MHZ(1)

#if SD_FAT_TYPE == 0
typedef File file_t;
typedef SdFat sd_t;
#elif SD_FAT_TYPE == 1
typedef SdFat32 sd_t;
typedef File32 file_t;
#elif SD_FAT_TYPE == 2
typedef SdExFat sd_t;
typedef ExFile file_t;
#elif SD_FAT_TYPE == 3
typedef SdFs sd_t;
typedef FsFile file_t;
#else // SD_FAT_TYPE
#error Invalid SD_FAT_TYPE
#endif // SD_FAT_TYPE



class FileController
{
public:
  FileController();
  ~FileController();
  file_t openSoundFile(const String &soundFileName, bool addHeader, uint8_t recordingTimeUs);
  file_t openDBFile(const String &dbAfilename, bool addHeader);

  void writeLine(file_t &file, const char *data);
  void writeSample(file_t &file, const byte *data);

private:
  sd_t _sd;
  void file_install();

};

#endif