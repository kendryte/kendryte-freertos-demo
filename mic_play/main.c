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
#include <devices.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "project_cfg.h"

handle_t i2s0;
handle_t i2s2;
const audio_format_t audio = {AUDIO_FMT_PCM, 16, 44100, 2};

void init_i2s(void)
{
    i2s_stop(i2s0);
    i2s_stop(i2s2);
    i2s_config_as_render(i2s2, &audio, 10, I2S_AM_RIGHT, 0xc);
    i2s_config_as_capture(i2s0, &audio, 10, I2S_AM_STANDARD, 0x3);
    i2s_start(i2s2);
    i2s_start(i2s0);
}

struct timeval get_time[2];

int main(void)
{
    printf("I2S0 receive , I2S2 play...\n");

    i2s0 = io_open("/dev/i2s0");
    i2s2 = io_open("/dev/i2s2");

    configASSERT(i2s0);
    configASSERT(i2s2);

    init_i2s();

    uint8_t *buffer_snd = NULL;
    uint8_t *buffer_rcv = NULL;
    size_t frames_snd = 0;
    size_t frames_rcv = 0;

    while (1)
    {
        i2s_get_buffer(i2s2, &buffer_snd, &frames_snd);
        i2s_get_buffer(i2s0, &buffer_rcv, &frames_rcv);
        memcpy(buffer_snd, buffer_rcv, frames_snd * 8);
        i2s_release_buffer(i2s2, frames_snd);
        i2s_release_buffer(i2s0, frames_rcv);
    }
    return 0;
}
