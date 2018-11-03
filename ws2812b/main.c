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
#include <stdlib.h>
#include <devices.h>
#include <sys/unistd.h>
#include <misc/ws2812b/ws2812b.h>
#include "project_cfg.h"

uintptr_t spi1;

int main()
{
    spi1 = io_open("/dev/spi1");
    configASSERT(spi1);
    
    handle_t ws2812b = spi_ws2812b_driver_install(spi1, 1);
    ws2812b_set_rgb_buffer(ws2812b, 0, 0xff);//blue
    ws2812b_set_rgb(ws2812b);
    sleep(2);
    ws2812b_set_rgb_buffer(ws2812b, 0, 0xff00);//red
    ws2812b_set_rgb(ws2812b);
    sleep(2);
    ws2812b_set_rgb_buffer(ws2812b, 0, 0xff0000);//green
    ws2812b_set_rgb(ws2812b);
    sleep(2);
    ws2812b_clear_rgb_buffer(ws2812b);
    ws2812b_set_rgb(ws2812b);
    while (1)
        ;
}
