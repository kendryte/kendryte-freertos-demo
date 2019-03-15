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
#include "spi_slave.h"
#include "spi_master.h"
#include <devices.h>
#include <stdio.h>
#include "project_cfg.h"

//#define MASTER_MODE

#ifdef MASTER_MODE
static uint8_t test_data[1024];
#else
static uint8_t slave_cfg[32];
static uint8_t test_data_tmp[1024];
#endif

int main(void)
{
#ifdef MASTER_MODE
    printf("spi master init\n");
    spi_master_init();
#else
    printf("spi slave init\n");
    *(uint32_t *)(&slave_cfg[8]) = (uint32_t)test_data_tmp;
    spi_slave_init(slave_cfg, 32);
#endif

    printf("System start\n");
#ifdef MASTER_MODE
    for (uint32_t i = 0; i < 8; i++)
        test_data[i] = i;
    spi_master_transfer(test_data, 0, 8, WRITE_CONFIG);
    for (uint32_t i = 0; i < 8; i++)
        test_data[i] = 0;
    spi_master_transfer(test_data, 0, 8, READ_CONFIG);
    for (uint32_t i = 0; i < 8; i++) {
        if (test_data[i] != (uint8_t)i)
            printf("%d: 0x%02x," ,i, test_data[i]);
    }
    printf("\n");

    uint32_t addr;
    spi_master_transfer((uint8_t *)&addr, 8, 4, READ_CONFIG);

    for (uint32_t i = 0; i < 8; i++)
        test_data[i] = i;
    spi_master_transfer(test_data, addr, 8, WRITE_DATA_BYTE);
    for (uint32_t i = 0; i < 8; i++)
        test_data[i] = 0;
    spi_master_transfer(test_data, addr, 8, READ_DATA_BYTE);
    for (uint32_t i = 0; i < 8; i++) {
        if (test_data[i] != (uint8_t)i)
            printf("%d: 0x%02x," ,i, test_data[i]);
    }
    printf("\n");

    for (uint32_t i = 0; i < 1024; i++)
        test_data[i] = i;
    spi_master_transfer(test_data, addr, 1024, WRITE_DATA_BLOCK);
    for (uint32_t i = 0; i < 1024; i++)
        test_data[i] = 0;
    spi_master_transfer(test_data, addr, 1024, READ_DATA_BLOCK);
    for (uint32_t i = 0; i < 1024; i++) {
        if (test_data[i] != (uint8_t)i)
            printf("%d: 0x%02x," ,i, test_data[i]);
    }
    printf("\n");

    printf("test finish\n");
#endif
    while (1)
        ;
}

