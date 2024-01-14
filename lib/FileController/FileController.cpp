#include "FileController.h"

// Try to select the best SD card configuration.
#if HAS_SDIO_CLASS
#define SD_CONFIG SdioConfig(FIFO_SDIO)
#elif ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, DEDICATED_SPI, SPI_CLOCK)
#else // HAS_SDIO_CLASS
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SPI_CLOCK)
#endif // HAS_SDIO_CLASS

FileController::FileController()
{
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Mounting SD card...");

  FileController::file_install();
}

FileController::~FileController()
{
  Serial.println("[" + TimeController::getFormattedDateTime() + "] Unmounting SD card...");
  _sd.end();
}

void FileController::file_install()
{
  // printf("Initializing SD card...");

  if (!_sd.begin(SD_CONFIG))
  {
    printf("Card Mount Failed\n");

    throw std::runtime_error("Card Mount Failed");
  }

  uint32_t size = _sd.card()->sectorCount();
  if (size == 0)
  {
    printf("Can't determine the card size.\n");
    return;
  }

  // uint32_t sizeMB = 0.000512 * size + 0.5;
  Serial.println("[" + TimeController::getFormattedDateTime() + "] SD card size: " + String(size / 2048) + " MB");
}

file_t FileController::openSoundFile(const String &soundFileName, bool addHeader, uint8_t recordingTimeUs)
{
  file_t soundFile;

  int retry = 0; // initialize retry count
  while (retry < 5)
  { // retry up to 5 times
    if (!soundFile.open(soundFileName.c_str(), O_APPEND | O_WRITE | O_CREAT))
    {
      Serial.println("Failed to open sound file: " + soundFileName);
      retry++;
      delay(1000); // wait for 1 second before retrying
    }
    else
    {
      if (addHeader)
      {
        // Serial.println("If new file, add header");

        // if file size is 0, add header
        if (soundFile.size() == 0)
        {
          // Get the WAV header
          wav_header_t wavh = RecordController::getWavHeader(recordingTimeUs);

          // Write the WAV header to the file
          soundFile.write((uint8_t *)&wavh, sizeof(wavh));
          Serial.println("WAV header written");
        }
      }
      return soundFile;
    }
  }

  throw std::runtime_error("Failed to open file after 5 retries"); // throw an error if all retries failed
}

file_t FileController::openDBFile(const String &dbAfilename, bool addHeader)
{
  file_t dbAfile;

  int retry = 0; // initialize retry count
  while (retry < 5)
  { // retry up to 5 times
    if (!dbAfile.open(dbAfilename.c_str(), O_APPEND | O_WRITE | O_CREAT))
    {
      Serial.println("Failed to dbA file");
      retry++;
      delay(1000); // wait for 1 second before retrying
    }
    else
    {
      if (addHeader)
      {
        // Serial.println("Check if file is empty");
        // if file size is 0, add header
        if (dbAfile.size() == 0)
        {
          Serial.println("Adding header");
          String dbHeader = "Time,dbA\n";
          dbAfile.write((const byte *)dbHeader.c_str(), dbHeader.length());
        }
      }

      // Serial.println("dbA file created");
      return dbAfile;
    }
  }

  throw std::runtime_error("Failed to create file after 5 retries"); // throw an error if all retries failed
}

void FileController::writeLine(file_t &file, const char *data)
{
  file.write((const byte *)data, strlen(data));
  file.write('\n');
}

void FileController::writeSample(file_t &file, const byte *data)
{
  file.write(data, READ_LEN);
}
