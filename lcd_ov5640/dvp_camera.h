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
#ifndef _DVPH_H
#define _DVPH_H
#include <stdio.h>

#define DVP_WIDTH 320
#define DVP_HIGHT 240

extern volatile uint8_t dvp_finish_flag;
extern volatile uint8_t gram_mux;

extern uint32_t lcd_gram0[38400];
extern uint32_t lcd_gram1[38400];

void dvp_init();
#endif
