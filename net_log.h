/*****************************************************************************
*                                                                            *
*  Copyright (C) 2021, Liu An Lin.                                           *
*                                                                            *
*  Licensed under the Apache License, Version 2.0 (the "License");           *
*  you may not use this file except in compliance with the License.          *
*  You may obtain a copy of the License at                                   *
*                                                                            *
*      http://www.apache.org/licenses/LICENSE-2.0                            *
*                                                                            *
*  Unless required by applicable law or agreed to in writing, software       *
*  distributed under the License is distributed on an "AS IS" BASIS,         *
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  *
*  See the License for the specific language governing permissions and       *
*  limitations under the License.                                            *
*                                                                            *
*****************************************************************************/

#ifndef __NET_LOG_H__
#define __NET_LOG_H__
#include <stdio.h>
#include <string.h>


#if (defined(_WIN32) || defined(_WIN64) || defined(__MINGW32__) || defined(__MINGW64__))
    #define NET_LOG_OS_WINDOWS
    #ifdef __GNUC__
        #define net_log_filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
    #else
        #define net_log_filename(x) strrchr(x,'\\')?strrchr(x,'\\')+1:x
    #endif
#else
	#define NET_LOG_OS_LINUX
    #define net_log_filename(x) strrchr(x,'/')?strrchr(x,'/')+1:x
#endif

#ifdef NET_LOG_OS_LINUX
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>


typedef int net_log_socket_t;
typedef socklen_t net_log_socklen_t;
#define NET_LOG_INVALID_SOCKET (-1)

static inline void net_log_close_socket(net_log_socket_t s)
{
    shutdown(s, 2);
    close(s);
}
        
#else
#include <windows.h>
typedef int net_log_socklen_t;
typedef SOCKET net_log_socket_t;
#define NET_LOG_INVALID_SOCKET INVALID_SOCKET

static inline void net_log_close_socket(net_log_socket_t s)
{
    shutdown(s, 2);
    closesocket(s);
}
#endif

#ifndef NET_LOG_SERVER_IP
#define NET_LOG_SERVER_IP "127.0.0.1"
#endif

#ifndef NET_LOG_FLAG
#define NET_LOG_FLAG ""
#endif

#define NET_LOG_PORT (18598)

#ifdef __GNUC__ 
#endif


#ifdef NET_LOG_DISABLE
#define NET_LOG_DISABLE_DEBUG
#define NET_LOG_DISABLE_INFO
#define NET_LOG_DISABLE_ERR
#define NET_LOG_DISABLE_WARN
#endif

static inline void net_log_sendto(const char *log, int len)
{
    net_log_socket_t sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock == NET_LOG_INVALID_SOCKET)
    {
        return;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(NET_LOG_PORT);
    addr.sin_addr.s_addr = inet_addr(NET_LOG_SERVER_IP);
    sendto(sock, log, len, 0, (struct sockaddr *)&addr, sizeof(addr));
    net_log_close_socket(sock);
}


#ifndef NET_LOG_DISABLE_DEBUG
#define net_log_debug(...) do { \
    static char __net_log_buf[8192]; \
    int __net_log_len = snprintf(__net_log_buf, sizeof(__net_log_buf), "D " NET_LOG_FLAG "%s:%s:%d ", net_log_filename(__FILE__), __FUNCTION__, __LINE__); \
    if (__net_log_len > 0) { \
        int __net_log_r = snprintf(__net_log_buf + __net_log_len, sizeof(__net_log_buf) - __net_log_len, ##__VA_ARGS__); \
        if (__net_log_r > 0) { \
            net_log_sendto(__net_log_buf, __net_log_len + __net_log_r); \
            printf("%s", __net_log_buf); \
        } \
    } \
} while (0)
#else
#define net_log_debug(...)
#endif



#ifndef NET_LOG_DISABLE_INFO
#define net_log_info(...) do { \
    static char __net_log_buf[8192]; \
    int __net_log_len = snprintf(__net_log_buf, sizeof(__net_log_buf), "I " NET_LOG_FLAG "%s:%s:%d ", net_log_filename(__FILE__), __FUNCTION__, __LINE__); \
    if (__net_log_len > 0) { \
        int __net_log_r = snprintf(__net_log_buf + __net_log_len, sizeof(__net_log_buf) - __net_log_len, ##__VA_ARGS__); \
        if (__net_log_r > 0) { \
            net_log_sendto(__net_log_buf, __net_log_len + __net_log_r); \
            printf("%s", __net_log_buf); \
        } \
    } \
} while (0)

#else
#define net_log_info(...)
#endif




#ifndef NET_LOG_DISABLE_ERR
#define net_log_err(...) do { \
    static char __net_log_buf[8192]; \
    int __net_log_len = snprintf(__net_log_buf, sizeof(__net_log_buf), "E " NET_LOG_FLAG "%s:%s:%d ", net_log_filename(__FILE__), __FUNCTION__, __LINE__); \
    if (__net_log_len > 0) { \
        int __net_log_r = snprintf(__net_log_buf + __net_log_len, sizeof(__net_log_buf) - __net_log_len, ##__VA_ARGS__); \
        if (__net_log_r > 0) { \
            net_log_sendto(__net_log_buf, __net_log_len + __net_log_r); \
            printf("%s", __net_log_buf); \
        } \
    } \
} while (0)

#else
#define net_log_err(...)
#endif


#ifndef NET_LOG_DISABLE_WARN
#define net_log_warn(...) do { \
    static char __net_log_buf[8192]; \
    int __net_log_len = snprintf(__net_log_buf, sizeof(__net_log_buf), "W " NET_LOG_FLAG "%s:%s:%d ", net_log_filename(__FILE__), __FUNCTION__, __LINE__); \
    if (__net_log_len > 0) { \
        int __net_log_r = snprintf(__net_log_buf + __net_log_len, sizeof(__net_log_buf) - __net_log_len, ##__VA_ARGS__); \
        if (__net_log_r > 0) { \
            net_log_sendto(__net_log_buf, __net_log_len + __net_log_r); \
            printf("%s", __net_log_buf); \
        } \
    } \
} while (0)

#else
#define net_log_warn(...)
#endif

#endif