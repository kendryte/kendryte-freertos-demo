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
#include "k_tftp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ip_addr.h>

socklen_t addr_len = sizeof(struct sockaddr_in);
int block_size = DATA_SIZE;
size_t k_tftp_packet_size = sizeof(k_tftp_packet_t);
char *root_path = "/fs/0";

void tftp_get(char *server_ip, unsigned short port, char *remote_file, char *local_file)
{
    int sock;
    struct sockaddr_in server;
    K_TFTP_DBG("Connect to server at %s:%d \n", server_ip, port);
    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("Server socket could not be created.\n");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", server_ip);
        return;
    }
    
    k_tftp_packet_t snd_packet, rcv_packet;
    struct sockaddr_in sender;
    size_t rcv_size = 0;
    uint32_t time_wait_data = 0;
    uint16_t block = 1;

    snd_packet.cmd = htons((uint16_t)CMD_RRQ);
    char *p = snd_packet.data;
    sprintf(p - 2, "%s%c%s%c%d%c", remote_file, 0, "octet", 0, block_size, 0);
    K_TFTP_DBG("filename : %s \n", snd_packet.filename);
    sendto(sock, &snd_packet, k_tftp_packet_size, 0, (struct sockaddr*)&server, addr_len);

    char full_path[256];
    memset(full_path, 0, sizeof(full_path));
    strcpy(full_path, root_path);
    if(local_file[0] != '/')
    {
        strcat(full_path, "/");
    }
    strcat(full_path, local_file);
    K_TFTP_DBG("full_path : %s \n", full_path);
    FILE *fp = fopen(full_path, "w");
    if(fp == NULL)
    {
        printf("Create file \"%s\" error.\n", local_file);
        return;
    }

    snd_packet.cmd = htons((uint16_t)CMD_ACK);
    do{
        for(time_wait_data = 0; time_wait_data < PKT_RCV_TIMEOUT * PKT_MAX_RXMT; time_wait_data += 10000)
          {
            rcv_size = recvfrom(sock, &rcv_packet, k_tftp_packet_size, MSG_DONTWAIT, (struct sockaddr *)&sender, &addr_len);
            if(rcv_size > 0 && rcv_size < 4)
            {
                printf("Bad packet: rcv_size=%ld\n", rcv_size);
            }
            if(rcv_size >= 4 && rcv_packet.cmd == htons((uint16_t)CMD_DATA) && rcv_packet.block == htons(block))
            {
                K_TFTP_DBG("DATA: block=%d, data_size=%ld\n", ntohs(rcv_packet.block), rcv_size - 4);
                snd_packet.block = rcv_packet.block;
                sendto(sock, &snd_packet, sizeof(k_tftp_packet_t), 0, (struct sockaddr*)&sender, addr_len);
                fwrite(rcv_packet.data, 1, rcv_size - 4, fp);
                break;
            }
            usleep(10000);
        }
        if(time_wait_data >= PKT_RCV_TIMEOUT * PKT_MAX_RXMT)
        {
            printf("Wait for DATA #%d timeout.\n", block);
            fclose(fp);
        }
        block ++;
    }while(rcv_size == block_size + 4);

    fclose(fp);
}

void tftp_put(char *server_ip, unsigned short port, char *local_file)
{
    int sock;
    struct sockaddr_in server;
    K_TFTP_DBG("Connect to server at %s:%d \n", server_ip, port);
    if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        printf("Server socket could not be created.\n");
        return;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, server_ip, &server.sin_addr) <= 0)
    {
        printf("inet_pton error for %s\n", server_ip);
        return;
    }
    
    struct sockaddr_in sender;
    k_tftp_packet_t rcv_packet, snd_packet;
    size_t rcv_size = 0;
    uint32_t time_wait_ack;

    snd_packet.cmd = htons((uint16_t)CMD_WRQ);
    char *p = snd_packet.data;
    sprintf(p -2, "%s%c%s%c%d%c", local_file, 0, "octet", 0, block_size, 0);
    sendto(sock, &snd_packet, sizeof(k_tftp_packet_t), 0, (struct sockaddr*)&server, addr_len); 
    for(time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20000)
    {
        rcv_size = recvfrom(sock, &rcv_packet, k_tftp_packet_size, MSG_DONTWAIT, (struct sockaddr *)&sender, &addr_len);
        if(rcv_size > 0 && rcv_size < 4)
        {
            printf("Bad packet: rcv_size=%ld\n", rcv_size);
        }
        if(rcv_size >= 4 && rcv_packet.cmd == htons((uint16_t)CMD_ACK) && rcv_packet.block == htons(0))
        {
            break;
        }
        usleep(20000);
    }
    if(time_wait_ack >= PKT_RCV_TIMEOUT)
    {
        printf("Could not receive ACK from server.\n");
        return;
    }

    char full_path[256];
    memset(full_path, 0, sizeof(full_path));
    strcpy(full_path, root_path);
    if(local_file[0] != '/')
    {
        strcat(full_path, "/");
    }
    strcat(full_path, local_file);
    K_TFTP_DBG("full_path : %s \n", full_path);
    FILE *fp = fopen(full_path, "r");
    if(fp == NULL)
    {
        printf("File not exists!\n");
        return;
    }

    size_t snd_size = 0;
    int rxmt;
    uint16_t block = 1;
    snd_packet.cmd = htons((uint16_t)CMD_DATA);
    do{
        memset(snd_packet.data, 0, sizeof(snd_packet.data));
        snd_packet.block = htons(block);
        snd_size = fread(snd_packet.data, 1, block_size, fp);

        for(rxmt = 0; rxmt < PKT_MAX_RXMT; rxmt ++)
        {
            sendto(sock, &snd_packet, snd_size + 4, 0, (struct sockaddr*)&sender, addr_len);
            K_TFTP_DBG("Send %d\n", block);

            for(time_wait_ack = 0; time_wait_ack < PKT_RCV_TIMEOUT; time_wait_ack += 20000)
            {
                rcv_size = recvfrom(sock, &rcv_packet, k_tftp_packet_size, MSG_DONTWAIT, (struct sockaddr *)&sender, &addr_len);
                if(rcv_size > 0 && rcv_size < 4)
                {
                    printf("Bad packet: rcv_size=%ld\n", rcv_size);
                }
                if(rcv_size >= 4 && rcv_packet.cmd == htons((uint16_t)CMD_ACK) && rcv_packet.block == htons(block))
                {
                    break;
                }
                usleep(20000);
            }
            if(time_wait_ack < PKT_RCV_TIMEOUT)
            {
                break;
            }
            else
            {
                continue;
            }
        }
        if(rxmt >= PKT_MAX_RXMT)
        {
            printf("Could not receive from server.\n");
            return;
        }
        block ++;
    }while(snd_size == block_size);

    K_TFTP_DBG("\nSend file end.\n");
    fclose(fp);
}

