/* Copyright 2018 Canaan Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <encoding.h>
#include <devices.h>
#include <encoding.h>
#include "fft_soft.h"

#define FFT_N               512U
/**
 * FFT 16-bit registers lead to data overflow (-32768-32767),
 * FFT has nine layers, shift determines which layer needs shift operation to prevent overflow.
 * (for example, 0x1ff means 9 layers do shift operation; 0x03 means the first layer and the second layer do shift operation) 
 * If shifted, the result is different from the normal FFT.
 */
#define FFT_FORWARD_SHIFT   0x0U
#define FFT_BACKWARD_SHIFT  0x1ffU
#define PI                  3.14159265358979323846

typedef struct _complex_hard
{
    int16_t real;
    int16_t imag;
} complex_hard_t;

enum _complex_mode
{
    FFT_HARD = 0,
    FFT_SOFT = 1,
    FFT_COMPLEX_MAX,
} ;
    
enum _time_get
{
    TEST_START = 0,
    TEST_STOP = 1,
    TEST_TIME_MAX,
} ;

int16_t real[FFT_N];
int16_t imag[FFT_N];
float hard_power[FFT_N];
float soft_power[FFT_N];
float hard_angel[FFT_N];
float soft_angel[FFT_N];
uint64_t fft_out_data[FFT_N / 2];
uint64_t buffer_input[FFT_N];
uint64_t buffer_output[FFT_N];
struct timeval get_time[FFT_COMPLEX_MAX][FFT_DIR_MAX][TEST_TIME_MAX];

uint16_t get_bit1_num(uint32_t data)
{
    uint16_t num;
    for (num = 0; data; num++)
        data &= data - 1;
    return num;
}

int main(void)
{
    int32_t i;
    float tempf1[3];
    fft_data_t *output_data;
    fft_data_t *input_data;
    uint16_t bit1_num = get_bit1_num(FFT_FORWARD_SHIFT);
    complex_hard_t data_hard[FFT_N] = {0};
    complex data_soft[FFT_N] = {0};

    /* Data prepared for fft hard calculation and fft soft calculation. */
    for (i = 0; i < FFT_N; i++)
    {
        tempf1[0] = 0.3 * cosf(2 * PI * i / FFT_N + PI / 3) * 256;
        tempf1[1] = 0.1 * cosf(16 * 2 * PI * i / FFT_N - PI / 9) * 256;
        tempf1[2] = 0.5 * cosf((19 * 2 * PI * i / FFT_N) + PI / 6) * 256;
        data_hard[i].real = (int16_t)(tempf1[0] + tempf1[1] + tempf1[2] + 10);
        data_hard[i].imag = (int16_t)0;
        data_soft[i].real = data_hard[i].real;
        data_soft[i].imag = data_hard[i].imag;
    }

    /* Data format conversion for hard fft module. */
    for (int i = 0; i < FFT_N / 2; ++i)
    {
        input_data = (fft_data_t *)&buffer_input[i];
        input_data->R1 = data_hard[2 * i].real;
        input_data->I1 = data_hard[2 * i].imag;
        input_data->R2 = data_hard[2 * i + 1].real;
        input_data->I2 = data_hard[2 * i + 1].imag;
    }

    /* FFT hard calculation. */
    gettimeofday(&get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_START], NULL);
    fft_complex_uint16(FFT_FORWARD_SHIFT, FFT_DIR_FORWARD, buffer_input, FFT_N, buffer_output);
    gettimeofday(&get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_STOP], NULL);

    /* FFT soft calculation. */
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START], NULL);
    fft_soft(data_soft, FFT_N);
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP], NULL);

    /* Data format conversion get from hard fft module output. */
    for (i = 0; i < FFT_N / 2; i++)
    {
        output_data = (fft_data_t*)&buffer_output[i];
        data_hard[2 * i].imag = output_data->I1 ;
        data_hard[2 * i].real = output_data->R1 ;
        data_hard[2 * i + 1].imag = output_data->I2 ;
        data_hard[2 * i + 1].real = output_data->R2 ;
    }

    /* Compare the difference between hardware fft and software fft calculation. */
    printf("\n[hard fft real][soft fft real][hard fft imag][soft fft imag]\n");
    for (i = 0; i < FFT_N / 2; i++)
        printf("%3d:%7d %7d %7d %7d\n",
                i, data_hard[i].real, (int32_t)data_soft[i].real, data_hard[i].imag, (int32_t)data_soft[i].imag);

    /* Power calculation. */
    for (i = 0; i < FFT_N; i++)
    {
        hard_power[i] = sqrt(data_hard[i].real * data_hard[i].real + data_hard[i].imag * data_hard[i].imag) * 2;
        soft_power[i] = sqrt(data_soft[i].real * data_soft[i].real + data_soft[i].imag * data_soft[i].imag) * 2;
    }
    printf("\nhard power  soft power:\n");
    printf("%3d : %f  %f\n", 0, hard_power[0] / 2 / FFT_N * (1 << bit1_num), soft_power[0] / 2 / FFT_N);
    for (i = 1; i < FFT_N / 2; i++)
        printf("%3d : %f  %f\n", i, hard_power[i] / FFT_N * (1 << bit1_num), soft_power[i] / FFT_N);

    printf("\nhard phase  soft phase:\n");
    for (i = 0; i < FFT_N / 2; i++)
    {
        hard_angel[i] = atan2(data_hard[i].imag, data_hard[i].real);
        soft_angel[i] = atan2(data_soft[i].imag, data_soft[i].real);
        printf("%3d : %f  %f\n", i, hard_angel[i] * 180 / PI, soft_angel[i] * 180 / PI);
    }

    for (int i = 0; i < FFT_N / 2; ++i)
    {
        input_data = (fft_data_t *)&buffer_input[i];
        input_data->R1 = data_hard[2 * i].real;
        input_data->I1 = data_hard[2 * i].imag;
        input_data->R2 = data_hard[2 * i + 1].real;
        input_data->I2 = data_hard[2 * i + 1].imag;
    }

    /* IFFT hard calculation. */
    gettimeofday(&get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_START], NULL);
    fft_complex_uint16(FFT_BACKWARD_SHIFT, FFT_DIR_BACKWARD, buffer_input, FFT_N, buffer_output);
    gettimeofday(&get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_STOP], NULL);

    /* IFFT soft calculation. */
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START], NULL);
    ifft_soft(data_soft, FFT_N);
    gettimeofday(&get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP], NULL);

    /* Data format conversion get from hard fft module output. */
    for (i = 0; i < FFT_N / 2; i++)
    {
        output_data = (fft_data_t*)&buffer_output[i];
        data_hard[2 * i].imag = output_data->I1 ;
        data_hard[2 * i].real = output_data->R1 ;
        data_hard[2 * i + 1].imag = output_data->I2 ;
        data_hard[2 * i + 1].real = output_data->R2 ;
    }

    /* Compare the difference between hardware ifft and software ifft calculation. */
    printf("\n[hard ifft real][soft ifft real][hard ifft imag][soft ifft imag]\n");
    for (i = 0; i < FFT_N / 2; i++)
        printf("%3d:%7d  %7d %7d %7d\n",
            i, data_hard[i].real, (int32_t)data_soft[i].real, data_hard[i].imag, (int32_t)data_soft[i].imag);

    /* Compare time. */
    printf("[hard ][%d bytes][forward time = %ld us][backward time = %ld us]\n",
            FFT_N,
            ((get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_STOP].tv_sec -get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_STOP].tv_usec - get_time[FFT_HARD][FFT_DIR_FORWARD][TEST_START].tv_usec)),
            ((get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_STOP].tv_sec -get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_STOP].tv_usec - get_time[FFT_HARD][FFT_DIR_BACKWARD][TEST_START].tv_usec)));
    printf("[soft ][%d bytes][forward time = %ld us][backward time = %ld us]\n",
            FFT_N,
            (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_sec -get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_FORWARD][TEST_START].tv_usec),
            (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_sec -get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_sec) * 1000*1000 +
            (get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_STOP].tv_usec - get_time[FFT_SOFT][FFT_DIR_BACKWARD][TEST_START].tv_usec));
    while (1)
        ;
    return 0;
}
