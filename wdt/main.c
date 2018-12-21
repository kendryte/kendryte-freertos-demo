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
#include <unistd.h>
#include <devices.h>

enum _enable
{
    DISABLE = 0,
    ENABLE = 1,
} ;

handle_t wdt0;

int main(void)
{
    int timeout = 0;

    printf("wdt start!\n");
    wdt0 = io_open("/dev/wdt0");
    wdt_set_enable(wdt0, DISABLE);

    wdt_set_response_mode(wdt0, WDT_RESP_RESET);
    wdt_set_timeout(wdt0, 6*1e9); //6sec
    wdt_set_enable(wdt0, ENABLE);

    while(1)
    {
        timeout ++;
        if(timeout < 6)
            wdt_restart_counter(wdt0);
        sleep(1);
    }
}

