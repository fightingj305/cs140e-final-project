#pragma once 
#include "ads1115.h"

#define FLOAT_DENOM_BITS 16 // for fast processing in our isr, define all floats as fractions of 2^16

#define SAMPLING_RATE 50000

#define LOG_TABLE_SIZE 17
static const float log_table[LOG_TABLE_SIZE] = {
    200.0f,    // t=0.000, dark
    280.0f,    // t=0.067
    393.0f,    // t=0.133
    550.0f,    // t=0.200
    770.0f,    // t=0.267
    1080.0f,   // t=0.333
    1350.0f,   // t=0.400
    1700.0f,   // t=0.467
    2120.0f,   // t=0.533
    2650.0f,   // t=0.600
    3300.0f,   // t=0.667
    4200.0f,   // t=0.733
    5300.0f,   // t=0.800
    6700.0f,   // t=0.867
    8500.0f,   // t=0.933
    11000.0f,  // t=0.967
    15000.0f   // t=1.000, full harmonics
};

int32_t prev_val = 0;
volatile uint32_t current_alpha = 0;
volatile uint32_t current_gain = 0;

#define MAX_DELAY_MS 500
#define MIN_DELAY_MS 0

#define DELAY_MIX (1 << (FLOAT_DENOM_BITS - 1)) // 50% mix
#define DELAY_BUFFER_SIZE (SAMPLING_RATE * MAX_DELAY_MS / 1000)
int32_t delay_buffer[DELAY_BUFFER_SIZE];
uint32_t delay_write = 0;
volatile uint32_t delay_samples = 0;
volatile int32_t delay_feedback = 0;

#define MAX_CLIP_VALUE ((1 << 23) - 1)
#define MIN_CLIP_VALUE MAX_CLIP_VALUE/128

volatile int32_t clip_value = MAX_CLIP_VALUE;

#define KNOB_DEAD_ZONE ADS_MAX_VALUE/10

uint16_t apply_dead_zone(uint16_t ads_val) {
    return ads_val < KNOB_DEAD_ZONE ? 0 : (ads_val > ADS_MAX_VALUE - KNOB_DEAD_ZONE ? ADS_MAX_VALUE : ads_val);
}

float ads_to_cutoff_log(uint16_t ads_val) {
    float t = (float)ads_val / (float)ADS_MAX_VALUE;
    float scaled = t * (LOG_TABLE_SIZE - 1);
    int idx = (int)scaled;
    float frac = scaled - (float)idx;

    if (idx >= LOG_TABLE_SIZE - 1) return log_table[LOG_TABLE_SIZE - 1];
    return log_table[idx] + frac * (log_table[idx + 1] - log_table[idx]); // linear interpolation
}

void set_cutoff(uint16_t ads_val) {
    ads_val = apply_dead_zone(ads_val);
    uint32_t cutoff_hz = ads_to_cutoff_log (ADS_MAX_VALUE - ads_val);

    float alpha = cutoff_hz / (cutoff_hz + (float) SAMPLING_RATE/(2.0f * 3.14159f));
    current_alpha = (uint32_t)(alpha * (1 << FLOAT_DENOM_BITS));

    float alpha_max = 15000.0f / (15000.0f + (float)SAMPLING_RATE / (2.0f * 3.14159f));
    
    // gain = alpha_max / alpha so that at max cutoff gain = 1x
    float gain = 0.75f * alpha_max / (alpha > 0.001f ? alpha : 0.001f);
    // if (gain > 64.0f) gain = 64.0f;
    current_gain = (uint32_t)(gain * (1 << FLOAT_DENOM_BITS));
    
    // UART_Send_String("cutoff_hz: "); UART_Print_Int(cutoff_hz);
    // UART_Send_String("gain: "); UART_Print_Int(current_gain);
}

void set_overdrive(uint16_t ads_val) {
    // UART_Send_String("ads_value: "); UART_Print_Int(ads_val);
    ads_val = apply_dead_zone(ads_val);
    uint32_t offset = (MAX_CLIP_VALUE - MIN_CLIP_VALUE) * ((float)ads_val / (ADS_MAX_VALUE));
    clip_value = offset + MIN_CLIP_VALUE;
    // UART_Send_String("clip_value: "); UART_Print_Int(clip_value);
}

void set_delay_time(uint16_t ads_val) {
    ads_val = apply_dead_zone(ads_val);
    delay_samples = (SAMPLING_RATE * MIN_DELAY_MS / 1000) + (uint32_t)(ADS_MAX_VALUE - ads_val) * (SAMPLING_RATE * (MAX_DELAY_MS - MIN_DELAY_MS) / 1000) / ADS_MAX_VALUE;
    // UART_Send_String("delay_samples: "); UART_Print_Int(delay_samples);
}

void set_delay_feedback(uint16_t ads_val) {
    ads_val = apply_dead_zone(ads_val);
    delay_feedback = (int32_t) (((float)(ADS_MAX_VALUE - ads_val) * 0.75f / ADS_MAX_VALUE) * (1 << FLOAT_DENOM_BITS)); // disallow feedback above 75%
    // UART_Send_String("delay_feedback: "); UART_Print_Int(delay_feedback);
}

int32_t process_signal(int32_t signal) {
    // delay
    uint32_t read_idx = (delay_write + DELAY_BUFFER_SIZE - delay_samples) 
                        % DELAY_BUFFER_SIZE;
    int32_t delayed = delay_buffer[read_idx];

    delay_buffer[delay_write % DELAY_BUFFER_SIZE] = signal + 
        (int32_t)(((int64_t)delayed * delay_feedback) >> FLOAT_DENOM_BITS);
    delay_write++;

    if (delay_samples > 0) {
        signal = signal + (int32_t)(((int64_t)delayed * DELAY_MIX) >> FLOAT_DENOM_BITS);
    }

    // distortion and normalization
    if (signal > clip_value)  signal =  clip_value;
    if (signal < -clip_value) signal = -clip_value;

    int32_t norm_gain = MAX_CLIP_VALUE / clip_value;
    // if (norm_gain > 32) norm_gain = 32;  // max 8x normalization gain
    signal *= norm_gain;
    
    // lowpass filter
    uint32_t alpha = current_alpha;
    signal = (int32_t)(
        ((int64_t)alpha * signal +
         ((int64_t)(1 << FLOAT_DENOM_BITS) - alpha) * prev_val) >> FLOAT_DENOM_BITS
    );

    // signal = (int32_t)(((int64_t)signal * current_gain) >> FLOAT_DENOM_BITS);
    prev_val = signal;
    return signal;
}
