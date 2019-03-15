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
#ifndef _PROJECT_CFG_H_
#define _PROJECT_CFG_H_
#include <pin_cfg.h>

#define SPI_SLAVE_INT_PIN       18
#define SPI_SLAVE_INT_IO        4
#define SPI_SLAVE_READY_PIN     22
#define SPI_SLAVE_READY_IO      5
#define SPI_SLAVE_CS_PIN        19
#define SPI_SLAVE_CLK_PIN       20
#define SPI_SLAVE_MOSI_PIN      21
#define SPI_SLAVE_MISO_PIN      21

#define SPI_MASTER_INT_PIN      28
#define SPI_MASTER_INT_IO       6
#define SPI_MASTER_CS_PIN       25
#define SPI_MASTER_CS_IO        7
#define SPI_MASTER_CLK_PIN      26
#define SPI_MASTER_MOSI_PIN     27
#define SPI_MASTER_MISO_PIN     27

const fpioa_cfg_t g_fpioa_cfg = 
{
    .version = PIN_CFG_VERSION,
    .functions_count = 5 + 4,
    .functions =
    {
        {SPI_SLAVE_CS_PIN, FUNC_SPI_SLAVE_SS},
        {SPI_SLAVE_CLK_PIN, FUNC_SPI_SLAVE_SCLK},
        {SPI_SLAVE_MOSI_PIN, FUNC_SPI_SLAVE_D0},
        {SPI_SLAVE_INT_PIN, FUNC_GPIOHS0 + SPI_SLAVE_INT_IO},
        {SPI_SLAVE_READY_PIN, FUNC_GPIOHS0 + SPI_SLAVE_READY_IO},
        
        {SPI_MASTER_CLK_PIN, FUNC_SPI0_SCLK},
        {SPI_MASTER_MOSI_PIN, FUNC_SPI0_D0},
        {SPI_MASTER_CS_PIN, FUNC_GPIOHS0 + SPI_MASTER_CS_IO},
        {SPI_MASTER_INT_PIN, FUNC_GPIOHS0 + SPI_MASTER_INT_IO},
    }
};

#endif
