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

#define TF_CS_GPIONUM   2
#define INT_GPIONUM   7
const fpioa_cfg_t g_fpioa_cfg = 
{
    .version = 1,
    .functions_count = 9,
    .functions =
    {
        {44, FUNC_SPI0_SS0},
        {41, FUNC_SPI0_SCLK},
        {43, FUNC_SPI0_D0},
        {42, FUNC_SPI0_D1},
        {46, FUNC_GPIOHS0 + INT_GPIONUM},
        {32, FUNC_GPIOHS0 + TF_CS_GPIONUM},
        {29, FUNC_SPI1_SCLK},
        {30, FUNC_SPI1_D0},
        {31, FUNC_SPI1_D1},
    }
};

const pin_cfg_t g_pin_cfg =
{
    .version = PIN_CFG_VERSION,
    .set_spi0_dvp_data = 0
};

#endif
