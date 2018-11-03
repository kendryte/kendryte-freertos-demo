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
#include <stdlib.h>
#include <string.h>
#include <devices.h>
#include <filesystem.h>
#include <storage/sdcard.h>
#include "project_cfg.h"

handle_t install_sdcard()
{
    handle_t spi, gpio;
    configASSERT(spi = io_open("/dev/spi0"));
    configASSERT(gpio = io_open("/dev/gpio0"));
    handle_t sd0 = spi_sdcard_driver_install(spi, gpio, 7);
    io_close(spi);
    io_close(gpio);
    return sd0;
}

int main()
{
    printf("Hello sd\n");
    char msg[]="k233333333333333333";
    char buffer[32];
    handle_t sd0 = install_sdcard();
    configASSERT(sd0);
    configASSERT(filesystem_mount("/fs/0/", sd0) == 0);
    io_close(sd0);

/************************* test find file************************************/
    find_find_data_t find_data;
    handle_t find = filesystem_find_first("/fs/0/", "*", &find_data);
    configASSERT(find);
    do
    {
        printf("%s\n", find_data.filename);
    } while (filesystem_find_next(find, &find_data));
    filesystem_file_close(find);

    printf("Done\n");

/************************* test filesystem operation ************************/
    handle_t file = filesystem_file_open("/fs/0/test_filesystem.txt", FILE_ACCESS_READ_WRITE, FILE_MODE_OPEN_ALWAYS);
    filesystem_file_write(file, (const uint8_t *)msg, strlen(msg)+1);
    filesystem_file_flush(file);
    filesystem_file_set_position(file, 0);
    filesystem_file_read(file, (uint8_t *)buffer, strlen(msg)+1);
    printf("test filesystem buffer : %s \n", buffer);
    filesystem_file_close(file);

/************************* test syscalls ************************************/
    FILE *stream;

    if((stream=fopen("/fs/0/test_syscalls.txt","w+"))==NULL)
    {
        fprintf(stderr,"Can not open file.\n");
        exit(-1);
    }
    fwrite(msg, 1, strlen(msg)+1, stream);
    fseek(stream,0,SEEK_SET);
    fread(buffer, 1, strlen(msg)+1, stream);
    printf("test syscalls buffer : %s\n", buffer);
    fclose(stream);
    while (1)
        ;
}
