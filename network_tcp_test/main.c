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

#define BUFFER_MAX 2048
#define SERVER_PORT 4321
#define SERVER_IPADDR "192.168.190.128"

void dhcp_task(void *args)
{
    handle_t netif = *(handle_t *)args;
    ip_address_t ipaddr, netmask, gateway;
    dhcp_state_t dhcp_state = network_interface_dhcp_pooling(netif);
    switch (dhcp_state)
    {
    case DHCP_ADDRESS_ASSIGNED:
    {
        printf("get dhcp ipaddr \n");
        network_get_addr(netif, &ipaddr, &netmask, &gateway);
        printf("get ipaddr:\n %d.%d.%d.%d \n", ipaddr.data[0], ipaddr.data[1], ipaddr.data[2], ipaddr.data[3]);
        printf("get mask:\n %d.%d.%d.%d \n", netmask.data[0], netmask.data[1], netmask.data[2], netmask.data[3]);
        printf("get gw:\n %d.%d.%d.%d \n", gateway.data[0], gateway.data[1], gateway.data[2], gateway.data[3]);
    }
    break;
    case DHCP_TIMEOUT:
    {
        printf("dhcp timeout \n");
    }
    break;
    default:
        printf("dhcp fail \n");
    break;
    }
    vTaskDelete(NULL);
}

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

    network_interface_set_enable(netif, true);
    network_interface_set_as_default(netif);
    io_close(spi);
    io_close(gpio);

    //xTaskCreate(dhcp_task, "dhcp task", 10240, &netif, 3, NULL);
}

void dns_test_task(void)
{
    char *host= "www.baidu.com";
    hostent_t hostent;
    network_socket_gethostbyname(host, &hostent);

    printf("get the host:%s addr\n",host);
    printf("name:%s\n",hostent.h_name);
    printf("type:%s\n",hostent.h_addrtype==AF_INTERNETWORK?"AF_INTERNETWORK":"FAIL");
    printf("ip legnth:%d\n",hostent.h_length); 
    int i = 0;
    for(i=0; ; i++)
    {
        if(hostent.h_addr_list[i] != NULL)
        {
            uint8_t *p = hostent.h_addr_list[i];
            printf("IP:%d.%d.%d.%d\n",p[0], p[1], p[2], p[3]);
        }
        else
        {
            break;
        }
    }
    for(i=0; ;i++)
    {
        if(hostent.h_aliases[i] != NULL)
        {
            printf("alias %d:%s\n",i,hostent.h_aliases[i]);
        }
        else
        {
            break;
        }
    }

    vTaskDelete(NULL);
}

void tcp_client_task(void)
{
    handle_t sock;
    char  buffer[BUFFER_MAX];
    socket_address_t  server_addr;
    
    sock = network_socket_open(AF_INTERNETWORK, SOCKET_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.family = AF_INTERNETWORK;
    network_socket_addr_parse(SERVER_IPADDR, SERVER_PORT, server_addr.data);

    network_socket_connect(sock, &server_addr);

    printf("send msg to server: \n");
    strcpy(buffer, "test network tcp");
    network_socket_send(sock, (const uint8_t *)buffer, strlen(buffer), MESSAGE_NORMAL);
    network_socket_shutdown(sock, SOCKSHTDN_BOTH);
    vTaskDelete(NULL);
}

void tcp_server_task()
{
    char  buffer[BUFFER_MAX];
    int  receive_size = 0;

    handle_t sock;
    handle_t client_sock;
    socket_address_t  server_addr;
    socket_address_t  client_addr;

    sock = network_socket_open(AF_INTERNETWORK, SOCKET_STREAM, 0);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.family = AF_INTERNETWORK;

    network_socket_addr_parse("0.0.0.0", SERVER_PORT, server_addr.data);

    network_socket_bind(sock, &server_addr);

    network_socket_listen(sock, 10);

    printf("======receive client request======\n");
    while (1)
    {
        client_sock = network_socket_accept(sock, &client_addr);
        receive_size = network_socket_receive(client_sock, (uint8_t *)buffer, sizeof(buffer), MESSAGE_NORMAL);

        buffer[receive_size] = '\0';

        char ipaddr[16];
        int port = 0;
        network_socket_addr_to_string(client_addr.data, ipaddr, &port);

        printf("recv msg from %s:%d\n%s\n", ipaddr, port, buffer);
        network_socket_shutdown(client_sock, SOCKSHTDN_BOTH);
    }
    network_socket_shutdown(sock, SOCKSHTDN_BOTH);
}

int main()
{
    init_network();
    vTaskDelay(500);
    xTaskCreate(dns_test_task, "dns test", 10240, NULL, 3, NULL);
#if 0
    xTaskCreate(tcp_server_task, "tcp server", 10240, NULL, 3, NULL);
#else
    xTaskCreate(tcp_client_task, "tcp client", 10240, NULL, 3, NULL);
#endif

    while (1)
        ;
}
