#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#define MAX_BUF_SIZE (1024)

char buf_send[MAX_BUF_SIZE];
char buf_recv[MAX_BUF_SIZE];

int main(int argc, char const *argv[])
{
    if(argc != 2)
    {
        printf("Usage:\n\t%s <ip address>\n", argv[0]);
        return -1;
    }

     int sockfd = socket(AF_INET,SOCK_STREAM,0);
     if (0 > sockfd)
     {
          perror("socket");
          return -1;
     }
     struct sockaddr_in addr = {};
     addr.sin_family = AF_INET;
     addr.sin_port = htons(4321);
     addr.sin_addr.s_addr = inet_addr(argv[1]);

    if (connect(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        printf("connect error: %s(errno: %d)\n", strerror(errno), errno);
        return 0;
    }
    socklen_t addr_len = sizeof(struct sockaddr_in);
    uint32_t i = 0;
    uint32_t packet_num = 0;
    for (i = 0; i < MAX_BUF_SIZE; i++) {
        buf_send[i] = 'a' + (i - 20) % 26;
    }
     while(1)
     {
          send(sockfd,buf_send,sizeof(buf_send),0);

          recv(sockfd,buf_recv,sizeof(buf_recv),0);
          printf("Recv%d:%s\n",packet_num++, buf_recv);
          memset(buf_recv, 0, MAX_BUF_SIZE);
     }
     close(sockfd);
     return 0;
}
