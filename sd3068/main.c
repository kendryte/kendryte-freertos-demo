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
#include <stdio.h>
#include <devices.h>
#include <task.h>
#include "sd3068.h"
#include "project_cfg.h"

handle_t i2c0;
handle_t i2c_sd3068;
struct time_t time_now;

void vTask1()
{
    while (1)
    {
        sd3068_get_time(&time_now);
        printf("%4d-%02d-%02d %02d:%02d:%02d\n", time_now.year + 2000,
            time_now.month, time_now.day, time_now.hour,
            time_now.min, time_now.sec);
            vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

void vTask2()
{
    uint8_t data_test[SD3068_USER_RAM_LEN], index;
    printf("write data\n");
    for (index = 0; index < SD3068_USER_RAM_LEN; index++)
        data_test[index] = index;
    sd3068_write_data(0, data_test, SD3068_USER_RAM_LEN);
    while (1)
    {
        printf("read data\n");
        for (index = 0; index < SD3068_USER_RAM_LEN; index++)
            data_test[index] = 0;
        sd3068_read_data(0, data_test, SD3068_USER_RAM_LEN);
        for (index = 0; index < SD3068_USER_RAM_LEN; index++)
        {
            if (data_test[index] != index)
            {
                configASSERT(!"i2c master test error\n");
            }
        }
        printf("i2c master test pass\n");
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

int32_t main(void)
{
    i2c0 = io_open("/dev/i2c0");
    configASSERT(i2c0);
    i2c_sd3068 = i2c_get_device(i2c0, SD3068ADDR, SD3068WIDTH);

    sd3068_init(i2c_sd3068);

    printf("write enable\n");
    sd3068_write_enable();

    printf("set time\n");
    time_now.year = 18;
    time_now.month = 10;
    time_now.day = 13;
    time_now.hour = 20;
    time_now.min = 0;
    time_now.sec = 0;
    sd3068_set_time(time_now);

    xTaskCreate(vTask2, "vTask2", 256, NULL, 2, NULL);
    xTaskCreate(vTask1, "vTask1", 1024, NULL, 3, NULL);

    while (1)
        ;
}
