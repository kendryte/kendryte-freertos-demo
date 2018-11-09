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
#include <string.h>
#include <FreeRTOS.h>
#include <task.h>
#include "lcd.h"

uint32_t lcd_gram[LCD_X_MAX * LCD_Y_MAX / 2] __attribute__((aligned(128)));

int main(void)
{
    printf("lcd test\n");
    lcd_init();
    memset(lcd_gram, 0xc0, LCD_X_MAX * LCD_Y_MAX * 2);
    lcd_clear(GREEN);
    lcd_draw_picture(100, 0, 120, 240, lcd_gram);
    lcd_draw_string(100, 50, "canaan kendryte",  BLUE);
    lcd_draw_string(100, 90, "hello AI world", NAVY);
    while (1)
        ;
}
