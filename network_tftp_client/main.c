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
#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "project_cfg.h"
#include <network/dm9051.h>
#include <devices.h>
#include <network.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <filesystem.h>
#include <storage/sdcard.h>
#include "k_tftp_client.h"

#define BUFFER_MAX 2048
#define SERVER_PORT 3333
#define SERVER_IPADDR "192.168.190.128"

void init_network()
{
    handle_t spi = io_open("/dev/spi0");
    handle_t gpio = io_open("/dev/gpio0");

    mac_address_t mac = { .data = {0x04,0x02,0x35,0x00,0x00,0x01} };
    handle_t dm9051 = dm9051_driver_install(spi, 1, gpio, 7, &mac);
    configASSERT(dm9051);

    network_init();
    ip_address_t ipaddr = { .family = AF_INTERNETWORK,.data = {192,168,190,30} };
    ip_address_t netmask = { .family = AF_INTERNETWORK,.data = {255,255,255,0} };
    ip_address_t gateway = { .family = AF_INTERNETWORK,.data = {192,168,190,1} };
    handle_t netif = network_interface_add(dm9051, &ipaddr, &netmask, &gateway);
    configASSERT(netif);
    puts("netif init");
    io_close(spi);
    io_close(gpio);
    
    network_interface_set_enable(netif, true);
    network_interface_set_as_default(netif);
}

handle_t install_sdcard()
{
    handle_t spi, gpio;
    configASSERT(spi = io_open("/dev/spi1"));
    configASSERT(gpio = io_open("/dev/gpio0"));
    handle_t sd0 = spi_sdcard_driver_install(spi, gpio, 2);
    io_close(spi);
    io_close(gpio);
    return sd0;
}

void tftp_client_task(void)
{
    tftp_get("192.168.190.28", 69, "test_kd.txt", "test_kd.txt");
    tftp_put("192.168.190.28", 69, "lena320.bmp");

    while (1)
        ;
}

int main()
{
    printf("test tftp client\n");
    handle_t sd0 = install_sdcard();
    configASSERT(sd0);
    configASSERT(filesystem_mount("/fs/0/", sd0) == 0);
    io_close(sd0);

    init_network();
    vTaskDelay(200);

    xTaskCreate(tftp_client_task, "tftp client", 10240, NULL, 3, NULL);

    while (1)
        ;
}
