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
#include <devices.h>
#include <unistd.h>

handle_t rtc0;
struct tm get_time;
const struct tm set_time =
{
    .tm_sec = 59,
    .tm_min = 22,
    .tm_hour = 17,
    .tm_mday = 8,
    .tm_mon = 11 - 1,
    .tm_year = 2018 - 1900,
    .tm_wday = 4,
    .tm_yday = -1,
    .tm_isdst = -1,
};

int main(void)
{
    rtc0 = io_open("/dev/rtc0");
    configASSERT(rtc0);

    rtc_set_datetime(rtc0, &set_time);

    while(1)
    {
        sleep(1);
        rtc_get_datetime(rtc0, &get_time);
        printf("%4d-%d-%d %d:%d:%d\n", get_time.tm_year + 1900, get_time.tm_mon + 1, get_time.tm_mday, get_time.tm_hour, get_time.tm_min, get_time.tm_sec);
    }
    return 0;
}
