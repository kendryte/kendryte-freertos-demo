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
int isr_status;

void gpio_isr()
{
    isr_status ++;
}

int main()
{
    printf("test gpiohs interrupt\n");
    gio = io_open("/dev/gpio0");
    configASSERT(gio);

    gpio_set_drive_mode(gio, 2, GPIO_DM_INPUT);
    gpio_set_drive_mode(gio, 3, GPIO_DM_OUTPUT);
    gpio_set_pin_value(gio, 3, GPIO_PV_HIGH);

    //gpio_set_pin_edge(gio, 2, GPIO_PE_FALLING); //falling edge trigger
    gpio_set_pin_edge(gio, 2, GPIO_PE_BOTH); //double edge trigger

    gpio_set_on_changed(gio, 2, gpio_isr, NULL); //register interrupt service

    while (1)
    {
        static int val = 0;
        gpio_set_pin_value(gio, 3, val = !val);
        vTaskDelay(1000 / portTICK_RATE_MS);
        printf("isr_status = %d \n", isr_status);
    }
}
