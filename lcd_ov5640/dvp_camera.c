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
#include <sys/unistd.h>
#include <devices.h>
#include "dvp_camera.h"

enum _data_for
{
    DATA_FOR_AI = 0,
    DATA_FOR_DISPLAY = 1,
} ;

enum _enable
{
    DISABLE = 0,
    ENABLE = 1,
} ;

volatile uint8_t dvp_finish_flag;
volatile uint8_t gram_mux;
handle_t file_dvp;
uint32_t lcd_gram0[38400] __attribute__((aligned(64)));
uint32_t lcd_gram1[38400] __attribute__((aligned(64)));

void sensor_restart()
{
    dvp_set_signal(file_dvp, DVP_SIG_POWER_DOWN, 1);
    usleep(200 * 1000);
    dvp_set_signal(file_dvp, DVP_SIG_POWER_DOWN, 0);
    usleep(200 * 1000);
    dvp_set_signal(file_dvp, DVP_SIG_RESET, 0);
    usleep(200 * 1000);
    dvp_set_signal(file_dvp, DVP_SIG_RESET, 1);
    usleep(200 * 1000);
}

extern volatile uint8_t refresh_flag;

void on_irq_dvp(dvp_frame_event_t event, void* userdata)
{
    switch (event)
    {
        case VIDEO_FE_BEGIN:
            dvp_enable_frame(file_dvp);
            break;
        case VIDEO_FE_END:
            dvp_set_output_attributes(file_dvp, DATA_FOR_DISPLAY, VIDEO_FMT_RGB565, gram_mux ? lcd_gram0 : lcd_gram1);
            dvp_finish_flag = 1;
            break;
        default:
            configASSERT(!"Invalid event.");
    }
}

void dvp_init()
{
    file_dvp = io_open("/dev/dvp0");
    configASSERT(file_dvp);
    sensor_restart();
    dvp_xclk_set_clock_rate(file_dvp, 20000000); /* 20MHz XCLK*/
    dvp_config(file_dvp, DVP_WIDTH, DVP_HIGHT, DISABLE);

    dvp_set_output_enable(file_dvp, DATA_FOR_AI, ENABLE);
    dvp_set_output_enable(file_dvp, DATA_FOR_DISPLAY, ENABLE);

    dvp_set_output_attributes(file_dvp, DATA_FOR_DISPLAY, VIDEO_FMT_RGB565, (void*)lcd_gram0);

    dvp_set_output_attributes(file_dvp, DATA_FOR_AI, VIDEO_FMT_RGB24_PLANAR, (void*)0x40600000);

    dvp_set_frame_event_enable(file_dvp, VIDEO_FE_END, DISABLE);
    dvp_set_frame_event_enable(file_dvp, VIDEO_FE_BEGIN, DISABLE);

    dvp_set_on_frame_event(file_dvp, on_irq_dvp, NULL);

    dvp_set_frame_event_enable(file_dvp, VIDEO_FE_END, ENABLE);
    dvp_set_frame_event_enable(file_dvp, VIDEO_FE_BEGIN, ENABLE);
}
