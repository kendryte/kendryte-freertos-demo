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
#include "project_cfg.h"
#include "w25qxx.h"

#define TEST_START_ADDR (0U)
#define TEST_NUMBER (0x100U)
uint8_t data_buf_send[TEST_NUMBER];
uint8_t data_buf_recv[TEST_NUMBER];
handle_t spi3;

void vTask1()
{
    int32_t index = 0;
    int32_t page_addr = TEST_START_ADDR;

    for (index = 0; index < TEST_NUMBER; index++)
        data_buf_send[index] = (uint8_t)(index);
    while (1)
    {
        w25qxx_write_data(page_addr, data_buf_send, TEST_NUMBER);
        page_addr += TEST_NUMBER;
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void vTask2()
{
    int32_t index = 0;
    int32_t page_addr = TEST_START_ADDR;
    for (index = 0; index < TEST_NUMBER; index++)
        data_buf_recv[index] = 0;
    while (1)
    {
        w25qxx_read_data(page_addr, data_buf_recv, TEST_NUMBER);
        for (index = 0; index < TEST_NUMBER; index++)
        {
            if (data_buf_recv[index] != (uint8_t)index)
            {
                printf("read : %d write : %d\n", data_buf_recv[index], index);
                configASSERT(!" read err");
            }
        }
        printf("%X Test OK\n", page_addr);
        page_addr += TEST_NUMBER;
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

int main(void)
{
    spi3 = io_open("/dev/spi3");
    configASSERT(spi3);
    w25qxx_init(spi3);

    xTaskCreate(vTask1, "vTask1", 1024, NULL, 3, NULL);
    xTaskCreate(vTask2, "vTask2", 1024, NULL, 2, NULL);

    while(1)
        ;
    return 0;
}
