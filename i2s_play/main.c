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
#include "i2s_play_pcm.h"

#define min(a, b) ((a) < (b) ? (a) : (b))

handle_t i2s0;
const audio_format_t audio = { AUDIO_FMT_PCM, 16, 44100, 2 };

void init_i2s(void)
{
    i2s_stop(i2s0);
    i2s_config_as_render(i2s0, &audio, 10, I2S_AM_RIGHT, 0xc);
    i2s_start(i2s0);
}

int main(void)
{
    printf("i2s play ...\n");

    uint8_t *buffer = NULL;
    size_t block_align = audio.bits_per_sample / 8 * audio.channels;
    size_t total_frames = sizeof(test_pcm) / block_align;

    i2s0 = io_open("/dev/i2s2");
    configASSERT(i2s0);
    init_i2s();
    while (1)
    {
        size_t offset = 0;
        size_t reset_frames = total_frames;
        while (reset_frames)
        {
            size_t frames;
            i2s_get_buffer(i2s0, &buffer, &frames);
            frames = min(frames, reset_frames);
            size_t buffer_size = frames * block_align;
            memcpy(buffer, (const uint8_t *)test_pcm + offset, buffer_size);
            i2s_release_buffer(i2s0, frames);

            offset += buffer_size;
            reset_frames -= frames;
        }
    }
    return 0;
}
