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
#include <sys/socket.h>
#include <errno.h>
#include <sys/ip_addr.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semphr.h>
#include <stdlib.h>

#define BUFFER_MAX 2048
#define SERVER_PORT 4321
#define _BACKLOG_ 10
#define SERVER_IPADDR "192.168.191.128"
#define MAX_BUF_SIZE 1024

char buf[MAX_BUF_SIZE];

void init_network()
{
    handle_t spi = io_open("/dev/spi0");
    handle_t gpio = io_open("/dev/gpio0");

    mac_address_t mac = { .data = {0x04,0x02,0x35,0x00,0x00,0x01} };
    handle_t dm9051 = dm9051_driver_install(spi, 1, gpio, 7, &mac);
    configASSERT(dm9051);
    puts("DRV init");

    network_init();
    ip_address_t ipaddr = { .family = AF_INTERNETWORK,.data = {192,168,191,30} };
    ip_address_t netmask = { .family = AF_INTERNETWORK,.data = {255,255,255,0} };
    ip_address_t gateway = { .family = AF_INTERNETWORK,.data = {192,168,191,1} };
    handle_t netif = network_interface_add(dm9051, &ipaddr, &netmask, &gateway);
    configASSERT(netif);
    puts("netif init");

    network_interface_set_enable(netif, true);
    network_interface_set_as_default(netif);
}

void tcp_server(void *arg)
{
    int client_sock = *((int *)arg);
    int receive_size;
    int ret;
    
    while(1)
    {  
        receive_size = recv(client_sock, buf, MAX_BUF_SIZE, 0);
        if (receive_size < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                receive_size = 0;
            }
            else
            {
                printf("read error = %d , %s (errno: %d)\n", receive_size, strerror(errno), errno);
                break;
            }
        }

        ret = send(client_sock,buf,MAX_BUF_SIZE, 0);
        if (ret < 0)
        {
            if (errno == EWOULDBLOCK)
            {
                ret = 0;
            }
            else
            {
                printf("write error = %d, %s\n", ret, strerror(errno));
                break;
            }
        }
    }
    close(client_sock);
}

void task_list(void *arg)
{
    char buffer[2048];
    for(;;)
    {
        vTaskDelay(10000 / portTICK_RATE_MS);
        vTaskList((char *)&buffer);
        printf("task_name   task_state  priority   stack  task_num\n");
        printf("%s\n", buffer);   
    }
    vTaskDelete(NULL);
    return;
}

void task_server(void *arg)
{
    int sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0)
    {
        printf("socket()\n");
    }
    struct sockaddr_in server_socket;
    struct sockaddr_in client_addr;
    char *client_ipaddr;
    pthread_t thread_id;  
    bzero(&server_socket,sizeof(server_socket));
    server_socket.sin_family=AF_INET;
    server_socket.sin_addr.s_addr=htonl(IPADDR_ANY);
    server_socket.sin_port=htons(SERVER_PORT);
    if(bind(sock,(struct sockaddr*)&server_socket,sizeof(struct sockaddr_in))<0)
    {
        printf("bind()\n");
        close(sock);
        return ;
    }
    if(listen(sock,_BACKLOG_)<0)
    {
        printf("listen()\n");
        close(sock);
        return ;
    }
    printf("success\n");

    for(;;)
    {
        socklen_t len=0;
        int client_sock=accept(sock,(struct sockaddr *)&client_addr, &len);
        if(client_sock<0)
        {
            printf("accept() error\n");
            continue;
        }
        client_ipaddr = (char *)inet_ntoa(client_addr.sin_addr);

        printf("get connect,ip is%s\n",client_ipaddr);
        pthread_create(&thread_id, NULL, (void *)tcp_server, &client_sock); 
    }
    close(sock);
    vTaskDelete(NULL);
}

int main()
{
    init_network();
    xTaskCreate(task_list, "task_list", 10240, NULL, 0, NULL);
    xTaskCreate(task_server, "task_server", 10240, NULL, 0, NULL);
    while(1)
        ;
    return 0;
}


