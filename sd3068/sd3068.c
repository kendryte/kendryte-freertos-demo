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
#include <stdlib.h>
#include <string.h>
#include "sd3068.h"

uintptr_t i2c_adapter;

void sd3068_init(uintptr_t i2c_dr)
{
    i2c_adapter = i2c_dr;
}

static uint8_t hex2bcd(uint8_t data)
{
    return data / 10 * 16 + data % 10;
}

static uint8_t bcd2hex(uint8_t data)
{
    return data / 16 * 10 + data % 16;
}

int sd3068_write_reg(uint8_t reg, uint8_t* data_buf, size_t length)
{
    uint8_t *data = malloc(length + 1);
    *data = reg;
    memcpy(data + 1, data_buf, length);
    int ret = io_write(i2c_adapter, data, length + 1L);
    free(data);
    return ret;
}

int sd3068_read_reg(uint8_t reg, uint8_t *data_buf, size_t length)
{
    return i2c_dev_transfer_sequential(i2c_adapter, &reg, 1L, data_buf, length);
}

void sd3068_write_enable(void)
{
    uint8_t data[2];

    data[0] = 0xFF;
    data[1] = 0x80;
    sd3068_write_reg(0x10, &data[1], 1L);
    sd3068_write_reg(0x0F, &data[0], 1L);
}

void sd3068_write_disable(void)
{
    uint8_t data[2];

    data[0] = 0x7B;
    data[1] = 0;
    sd3068_write_reg(0x0F, data, 2L);
}

int sd3068_write_data(uint8_t addr, uint8_t* data_buf, size_t length)
{
    int ret = 0;
    addr = addr <= 69 ? addr : 69;
    length = length <= 70 - addr ? length : 70 - addr;
    ret = sd3068_write_reg(0x2C + addr, data_buf, length);

    return ret;
}

int sd3068_read_data(uint8_t addr, uint8_t* data_buf, size_t length)
{
    int ret = 0;
    addr = addr <= 69 ? addr : 69;
    length = length <= 70 - addr ? length : 70 - addr;
    ret = sd3068_read_reg(0x2C + addr, data_buf, length);

    return ret;
}

void sd3068_set_time(struct time_t time)
{
    uint8_t data[7];

    data[0] = hex2bcd(time.sec);
    data[1] = hex2bcd(time.min);
    data[2] = hex2bcd(time.hour) | 0x80;
    data[3] = hex2bcd(5);
    data[4] = hex2bcd(time.day);
    data[5] = hex2bcd(time.month);
    data[6] = hex2bcd(time.year);
    sd3068_write_reg(0x00, data, 7L);
}

void sd3068_get_time(struct time_t* time)
{
    uint8_t data[7];

    sd3068_read_reg(0x00, data, 7);
    time->sec = bcd2hex(data[0]);
    time->min = bcd2hex(data[1]);
    time->hour = bcd2hex(data[2] & 0x7F);
    time->day = bcd2hex(data[4]);
    time->month = bcd2hex(data[5]);
    time->year = bcd2hex(data[6]);
}
