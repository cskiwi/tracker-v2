#ifndef __RecordController_H
#define __RecordController_H
#include <Arduino.h>

#include <driver/i2s.h>

//
// Configuration
//

//
// I2S Setup
//
#define I2S_TASK_PRI 4
#define I2S_TASK_STACK 2048
#define I2S_PORT I2S_NUM_0

//
// I2S pins
//

// MIC 1
#define MIC_I2S_WS GPIO_NUM_22
#define MIC_I2S_SD GPIO_NUM_21
#define MIC_I2S_SCK GPIO_NUM_26

//
// Sampling
//
// #define SAMPLE_RATE (44100) // Hz, fixed to design of IIR filters
// #define SAMPLE_BITS (16)    // bits
// #define READ_LEN (SAMPLE_RATE * NUM_CHANNELS * (SAMPLE_BITS / 8)) // readlenght / second

#define SAMPLE_RATE (44100)
#define READ_LEN (SAMPLE_RATE * NUM_CHANNELS * (SAMPLE_BITS / 8)) // readlenght / second
#define NUM_CHANNELS (1)
#define SAMPLE_BITS (16)

#define SAMPLE_T char
#define GAIN_FACTOR 3.0

#define DMA_BANK_SIZE (SAMPLE_RATE / 8 / 16)
#define DMA_BANKS 32

// Set up the WAV header
struct wav_header_t
{
    char riff[4];         /* "RIFF"                                  */
    long flength;         /* file length in bytes                    */
    char wave[4];         /* "WAVE"                                  */
    char fmt[4];          /* "fmt "                                  */
    long chunk_size;      /* size of FMT chunk in bytes (usually 16) */
    short format_tag;     /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
    short num_chans;      /* 1=mono, 2=stereo                        */
    long srate;           /* Sampling rate in samples per second     */
    long bytes_per_sec;   /* bytes per second = srate*bytes_per_samp */
    short bytes_per_samp; /* 2=16-bit mono, 4=16-bit stereo          */
    short bits_per_samp;  /* Number of bits per sample               */
    char data[4];         /* "data"                                  */
    long dlength;         /* data length in bytes (filelength - 44)  */
};

class RecordController
{
public:
    RecordController();
    ~RecordController();
    SAMPLE_T *recordSample();
    static wav_header_t getWavHeader(const uint8_t recordingTimeUs);

private:
    void mic_i2s_install();
    void mic_i2s_set_pin();
    void i2s_adc_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len);
};

#endif