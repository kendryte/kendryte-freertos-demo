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
#include "project_cfg.h"

#define CHANNEL 0

handle_t gio;
handle_t pwm0;
handle_t pwm1;
handle_t timer;

void irq_time(void *userdata)
{
    static double cnt = 0.01;
    static int flag = 0;
    pwm_set_enable(pwm0, CHANNEL, 0);
    pwm_set_active_duty_cycle_percentage(pwm0, CHANNEL, cnt);
    pwm_set_enable(pwm0, CHANNEL, 1);
    flag ? (cnt -= 0.01) : (cnt += 0.01);
    if (cnt > 1.0)
    {
        cnt = 1.0;
        flag = 1;
    }
    else if (cnt < 0.0)
    {
        cnt = 0.0;
        flag = 0;
    }
}

int main()
{
    pwm0 = io_open("/dev/pwm0");
    configASSERT(pwm0);
    pwm_set_frequency(pwm0, 200000); //set 200KHZ
    pwm_set_active_duty_cycle_percentage(pwm0, CHANNEL, 0.5); //duty 50%
    pwm_set_enable(pwm0, CHANNEL, 1);

    timer = io_open("/dev/timer10");
    configASSERT(timer);

    timer_set_interval(timer, 1e7);
    timer_set_on_tick(timer, irq_time, NULL);
    timer_set_enable(timer, 1);

    while (1)
        ;
}
