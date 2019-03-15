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
#include <stdio.h>
#include <devices.h>

#define SPI_SLAVE_INT_PIN       18
#define SPI_SLAVE_INT_IO        4
#define SPI_SLAVE_READY_PIN     22
#define SPI_SLAVE_READY_IO      5
#define SPI_SLAVE_CS_PIN        19
#define SPI_SLAVE_CLK_PIN       20
#define SPI_SLAVE_MOSI_PIN      21
#define SPI_SLAVE_MISO_PIN      21

static handle_t gio;
static handle_t spi_slave;

int spi_slave_receive_hook(void *data)
{
    printf("%d\n", ((spi_slave_command_t *)data)->err);
    return 0;
}

int spi_slave_init(uint8_t *data, uint32_t len)
{
    gio = io_open("/dev/gpio0");
    configASSERT(gio);
    spi_slave = io_open("/dev/spi_slave");
    configASSERT(spi_slave);
    spi_slave_config(spi_slave, gio, SPI_SLAVE_INT_IO, SPI_SLAVE_READY_IO, 8, data, len, spi_slave_receive_hook);

    return 0;
}
