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
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include "project_cfg.h"

handle_t gio;

void vTask1()
{
    while (1)
    {
        int val = gpio_get_pin_value(gio, 2);

        printf("The PIN is %d\n", val);
        vTaskDelay(500 / portTICK_RATE_MS);
    }
}

void vTask2()
{
    while (1)
    {
        static int val = 0;

        gpio_set_pin_value(gio, 3, val = !val);
        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}

int main()
{
    gio = io_open("/dev/gpio0");
    configASSERT(gio);

    gpio_set_drive_mode(gio, 2, GPIO_DM_INPUT);
    gpio_set_drive_mode(gio, 3, GPIO_DM_OUTPUT);
    gpio_set_pin_value(gio, 3, GPIO_PV_HIGH);

    vTaskSuspendAll();
    xTaskCreate(vTask1, "vTask1", 512, NULL, 3, NULL);
    xTaskCreate(vTask2, "vTask2", 128, NULL, 2, NULL);
    if (!xTaskResumeAll())
    {
        taskYIELD();
    }

    while (1)
        ;
}
