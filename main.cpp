#include <stdio.h>
#include "net_log.h"


int main(int argc, char **argv)
{
    
#ifdef NET_LOG_OS_WINDOWS
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);
#endif

    net_log_socket_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == NET_LOG_INVALID_SOCKET)
    {
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NET_LOG_PORT);
    addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    
    int value = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&value, (net_log_socklen_t)sizeof(value));
    if (0 != bind(sock, (struct sockaddr *)&addr, sizeof(addr)))
    {
        net_log_close_socket(sock);
        return -1;
    }
    
    static char buf[32 * 1024];
    while (1)
    {
        struct sockaddr_in raddr;
        net_log_socklen_t addr_len = sizeof(raddr);
        int rlen = recvfrom(sock, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&raddr, &addr_len);
        if (rlen > 0)
        {
            buf[rlen] = 0;
            printf("%s", buf);
        }
    }
    
    net_log_close_socket(sock);
    return 0;
}
