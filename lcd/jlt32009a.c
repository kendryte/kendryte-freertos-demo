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
#include "jlt32009a.h"
#include "project_cfg.h"

uintptr_t gio;
uintptr_t spi0;
uintptr_t spi_dfs8;
uintptr_t spi_dfs16;
uintptr_t spi_dfs32;

void init_dcx()
{
    gio = io_open("/dev/gpio0");
    configASSERT(gio);
    gpio_set_drive_mode(gio, DCX_GPIONUM, GPIO_DM_OUTPUT);
    gpio_set_pin_value(gio, DCX_GPIONUM, GPIO_PV_HIGH);
}

void set_dcx_control()
{
    configASSERT(gio);
    gpio_set_pin_value(gio, DCX_GPIONUM, GPIO_PV_LOW);
}

void set_dcx_data()
{
    configASSERT(gio);
    gpio_set_pin_value(gio, DCX_GPIONUM, GPIO_PV_HIGH);
}

void spi_control_init()
{

    spi0 = io_open("/dev/spi0");
    configASSERT(spi0);
    spi_dfs8 = spi_get_device(spi0, "lcd", SPI_MODE_0 /*SPI_Mode_3*/, SPI_FF_OCTAL, 1 << SPI_SLAVE_SELECT /*chip select 3(CS3)*/, 8 /*frame length*/);
    spi_dev_config_non_standard(spi_dfs8, 8 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    
    spi_dfs16 = spi_get_device(spi0, "lcd", SPI_MODE_0 /*SPI_Mode_3*/, SPI_FF_OCTAL, 1 << SPI_SLAVE_SELECT /*chip select 3(CS3)*/, 16 /*frame length*/);
    spi_dev_config_non_standard(spi_dfs16, 16 /*instrction length*/, 0 /*address length*/, 0 /*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);
    
    spi_dfs32 = spi_get_device(spi0, "lcd", SPI_MODE_0 /*SPI_Mode_3*/, SPI_FF_OCTAL, 1 << SPI_SLAVE_SELECT /*chip select 3(CS3)*/, 32 /*frame length*/);
    spi_dev_config_non_standard(spi_dfs32, 0 /*instrction length*/, 32 /*address length*/, 0 /*wait cycles*/, SPI_AITM_AS_FRAME_FORMAT /*spi address trans mode*/);

    spi_dev_set_clock_rate(spi_dfs8, 6400000);
    spi_dev_set_clock_rate(spi_dfs16, 6400000);
    spi_dev_set_clock_rate(spi_dfs32, 6400000);
}

void tft_hard_init(void)
{
    init_dcx();
    spi_control_init();
}

void tft_write_command(uint8_t cmd)
{
    set_dcx_control();
    io_write(spi_dfs8, (const uint8_t *)(&cmd), 1);
}

void tft_write_byte(uint8_t* data_buf, uint32_t length)
{
    set_dcx_data();
    io_write(spi_dfs8, (const uint8_t *)(data_buf), length);
}

void tft_write_half(uint16_t* data_buf, uint32_t length)
{
    set_dcx_data();
    io_write(spi_dfs16, (const uint8_t *)(data_buf), length * 2);
}

void tft_write_word(uint32_t* data_buf, uint32_t length, uint32_t flag)
{
    set_dcx_data();
    io_write(spi_dfs32, (const uint8_t *)data_buf, length * 4);
}

void tft_fill_data(uint32_t* data_buf, uint32_t length)
{
    set_dcx_data();
    spi_dev_fill(spi_dfs32, 0, *data_buf, *data_buf, length - 1);
}
