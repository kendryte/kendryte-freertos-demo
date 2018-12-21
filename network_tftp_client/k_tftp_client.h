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
#include <stdint.h>

#define PKT_SND_TIMEOUT 12*1000*1000
#define PKT_RCV_TIMEOUT 3*1000*1000
#define PKT_MAX_RXMT    3
#define LINE_BUF_SIZE   1024
#define DATA_SIZE       512

//#define K_TFTP_DBUG
#ifdef K_TFTP_DBUG
#define K_TFTP_DBG(fmt, args...) printf(fmt, ##args)
#else
#define K_TFTP_DBG(fmt, args...)
#endif

typedef enum _k_tftp_cmd
{
    CMD_RRQ = 1,
    CMD_WRQ,
    CMD_DATA,
    CMD_ACK,
    CMD_ERROR
}k_tftp_cmd_t;

typedef struct _k_tftp_packet
{
    uint16_t cmd;
    union{
        uint16_t code;
        uint16_t block;
        uint16_t error;
        char filename[2];
    };
    char data[DATA_SIZE];
}k_tftp_packet_t;

void tftp_get(char *server_ip, unsigned short port, char *remote_file, char *local_file);
void tftp_put(char *server_ip, unsigned short port, char *local_file);