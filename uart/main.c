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
#include <string.h>
#include "project_cfg.h"

uintptr_t uart1;

int main()
{
    uint8_t recv = 0;
    uart1 = io_open("/dev/uart1");

    uart_config(uart1, 115200, 8, UART_STOP_1, UART_PARITY_NONE);

    char *hel = {"hello uart!\n"};
    io_write(uart1, (uint8_t *)hel, strlen(hel));

    while (1)
    {
        while (io_read(uart1, &recv, 1))
        {
            io_write(uart1, &recv, 1);
        }
    }
}
