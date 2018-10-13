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
/**
 * @file
 * @brief      sd3068 driver
 */
#ifndef _SD3068_H
#define _SD3068_H

#include <stdint.h>

#define SD3068ADDR              0x32
#define SD3068WIDTH             7
#define SD3068_USER_RAM_LEN     70

struct time_t
{
    uint32_t year : 6;
    uint32_t month : 4;
    uint32_t day : 5;
    uint32_t hour : 5;
    uint32_t min : 6;
    uint32_t sec : 6;
} __attribute__((packed, aligned(4)));

void sd3068_init(uintptr_t i2c_dr);
void sd3068_write_enable(void);
void sd3068_write_disable(void);
void sd3068_set_time(struct time_t time);
void sd3068_get_time(struct time_t* time);
int sd3068_write_data(uint8_t addr, uint8_t* data_buf, size_t length);
int sd3068_read_data(uint8_t addr, uint8_t* data_buf, size_t length);
int sd3068_write_reg(uint8_t reg, uint8_t* data_buf, size_t length);
int sd3068_read_reg(uint8_t reg, uint8_t* data_buf, size_t length);

#endif
