#include "RecordController.h"

RecordController::RecordController()
{
    Serial.println("[" + TimeController::getFormattedDateTime() + "] Starting I2S");

    RecordController::mic_i2s_install();
    RecordController::mic_i2s_set_pin();
    i2s_start(I2S_PORT);

    // first sample can be discarded
    size_t bytes_read = 0;
    SAMPLE_T *sample = recordSample(&bytes_read);

    // free sample memory
    free(sample);
}

RecordController::~RecordController()
{
    Serial.println("[" + TimeController::getFormattedDateTime() + "] stopping I2S");

    i2s_stop(I2S_PORT);
    i2s_driver_uninstall(I2S_PORT);
}

SAMPLE_T *RecordController::recordSample(size_t *bytes_read)
{
    char *samples = (char *)calloc(READ_LEN, sizeof(char));

    // check if memory was allocated
    if (samples == NULL)
    {
        Serial.println("Error: Memory not allocated.");
        return NULL;
    }

    i2s_read(I2S_PORT, (void *)samples, READ_LEN, bytes_read, portMAX_DELAY);
    i2s_adc_data_scale((uint8_t *)samples, (uint8_t *)samples, READ_LEN);

    return samples;
}

int RecordController::getRecordSize(const uint8_t recordingTimeSeconds)
{
    return NUM_CHANNELS * SAMPLE_RATE * SAMPLE_BITS / 8 * recordingTimeSeconds;
}

void RecordController::mic_i2s_install()
{
    const i2s_config_t i2s_config = {
        .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = i2s_bits_per_sample_t(SAMPLE_BITS),
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_STAND_I2S),
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = DMA_BANKS,
        .dma_buf_len = DMA_BANK_SIZE,
        .use_apll = true};

    esp_err_t result_install = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    // printf("I2S driver install: %d\n", result_install);
    assert(result_install == ESP_OK);

    esp_err_t result_sample = i2s_set_sample_rates(I2S_PORT, SAMPLE_RATE);
    // printf("I2S driver sample rates: %d\n", result_sample);
    assert(result_sample == ESP_OK);

    esp_err_t result_clk = i2s_set_clk(I2S_PORT, SAMPLE_RATE, SAMPLE_BITS, I2S_CHANNEL_MONO);
    assert(result_clk == ESP_OK);
}

void RecordController::mic_i2s_set_pin()
{
    // I2S pin mapping
    const i2s_pin_config_t pin_config = {
        .bck_io_num = MIC_I2S_SCK,
        .ws_io_num = MIC_I2S_WS,
        .data_out_num = I2S_PIN_NO_CHANGE, // not used
        .data_in_num = MIC_I2S_SD};

    esp_err_t result_pin = i2s_set_pin(I2S_PORT, &pin_config);
    // printf("I2S driver pin: %d\n", result_pin);
    assert(result_pin == ESP_OK);
}

void RecordController::i2s_adc_data_scale(uint8_t *d_buff, uint8_t *s_buff, uint32_t len)
{
    if (s_buff == NULL)
    {
        Serial.println("Error: s_buff pointer is NULL");
        return;
    }

    if (len % 2 != 0)
    {
        Serial.println("Error: len is not a multiple of 2");
        return;
    }

    float gain = GAIN_FACTOR;

    for (uint32_t i = 0; i < len; i += sizeof(int32_t))
    {
        int32_t sample = *((int32_t *)(s_buff + i));
        sample = (int32_t)(sample * gain);
        *((int32_t *)(d_buff + i)) = sample;
    }
}

wav_header_t RecordController::getWavHeader(const uint8_t recordingTimeUs)
{
    long file_length = recordingTimeUs * SAMPLE_RATE * NUM_CHANNELS * (I2S_BITS_PER_SAMPLE_16BIT / 8);
    long data_length = file_length - 44;

    wav_header_t wavh = {
        {'R', 'I', 'F', 'F'},
        file_length,                                 /* "RIFF"                                  */
        {'W', 'A', 'V', 'E'},                        /* file length in bytes                    */
        {'f', 'm', 't', ' '},                        /* "WAVE"                                  */
        16,                                          /* "fmt "                                  */
        1,                                           /* size of FMT chunk in bytes (usually 16) */
        (short)NUM_CHANNELS,                         /* 1=PCM, 257=Mu-Law, 258=A-Law, 259=ADPCM */
        SAMPLE_RATE,                                 /* 1=mono, 2=stereo                        */
        SAMPLE_RATE * NUM_CHANNELS * (READ_LEN / 2), /* Sampling rate in samples per second     */
        (short)(NUM_CHANNELS * (READ_LEN / 2)),      /* bytes per second = srate*bytes_per_samp */
        16,                                          /* 2=16-bit mono, 4=16-bit stereo          */
        {'d', 'a', 't', 'a'},                        /* Number of bits per sample               */
        data_length                                  /* "data"                                  */
    };                                               /* data length in bytes (filelength - 44)  */

    return wavh;
}

float RecordController::setGain(float gain)
{
    GAIN_FACTOR += gain;
    return GAIN_FACTOR;
}
