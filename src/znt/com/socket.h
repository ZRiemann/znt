/**
 * MIT License
 *
 * Copyright (c) 2018 Z.Riemann
 * https://github.com/ZRiemann/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the Software), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED AS IS, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM
 * , OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
#ifndef _ZCOM_SOCKET_H_
#define _ZCOM_SOCKET_H_

/**
 * @file socket.h
 * @brief <A brief description of what this file is.>
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-03 Z.Riemann found
 */
#include <zsi/base/type.h>

ZC_BEGIN

#ifdef ZSYS_WINDOWS
#include <windows.h>
#include <winsock2.h>

typedef SOCKET zsock_t;
typedef int socklen_t;
typedef SOCKADDR_IN zsockaddr_in;
typedef struct sockaddr ZSA;
#define ZINVALID_SOCKET INVALID_SOCKET
/* CAUTION: call WSAStartup and WSAClean up first; */

#else /* ZSYS_POSIX */
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
typedef int zsock_t;
typedef struct sockaddr_in zsockaddr_in;
typedef struct sockaddr ZSA;
#define ZINVALID_SOCKET -1

#endif /* ZSYS_WINDOWS */

#include <zsi/base/error.h>

#define ZTRACE_SOCKET 1

ZAPI int zsock_init(int v1, int v2); // windows WSAStartup
ZAPI int zsock_fini(); // Widnows WSACleanup

/**@fn zsock_t zsocket(int domain, int type, int protocol)
 * @brief create an endpint for communication
 * @param int domain [in] AF_INET/INET6/UNIX/...
 * @param int type [in] SOCK_STREAM/DGRAM/SEQPACKET/RAW/RDM/PACKET
 * @param int protocol [in] default 0
 */
ZAPI zsock_t zsocket(int domain, int type, int protocol);
ZAPI int zsockclose(zsock_t sock);
/*
 * active connect api
 */
ZAPI int zinet_addr(zsockaddr_in *addr, const char *host, uint16_t port);
ZAPI int zinet_str(zsockaddr_in *addr, char *host, uint16_t *port);
ZAPI int zgetpeername(zsock_t sock, zsockaddr_in *addr, char *host, uint16_t *port);
ZAPI int zconnect(zsock_t sock, const ZSA *addr, int len);
/* replace by inline definition
 * ZAPI int zrecv(zsock_t sock, char *buf, int len, int flags);
 * ZAPI int zsend(zsock_t sock, const char *buf, int len, int flags);
 */
/*
 * passive connect api
 */
ZAPI int zbind(zsock_t sock, const ZSA *addr, int len);
ZAPI int zlisten(zsock_t sock, int listenq); // listenq=1024
ZAPI zsock_t zaccept(zsock_t sock, ZSA *addr, int *addrlen);
/* select() -1 infinit 0 no wait >0 wait ms */
ZAPI int zselect(int maxfdp1, fd_set *read, fd_set *write, fd_set *except, struct timeval *timeout);
/* get/setoption() */
ZAPI int zsock_nonblock(zsock_t sock, int noblock);
/**@fn int recv_packet(sock_t sock, char *buf, int maxlen, int* offset, int *len, char *bitmask)
 * @brief recv a packet
 * @return ZOK - sock closed
 * @return ZAGAIN - recv ok but need more
 * @return ZFUN_FAIL - call system API failed
 * @return ret>0 - packet len
 * @note
 * buf - packet begin
 * offset - packet end + 1
 * len - next packet data
 */
ZAPI int zrecv_packet(zsock_t sock, char *buf, int maxlen, int* offset, int *len, char bitmask);

/**@fn int zconnectx(zsock_t sock, const char *host, uint 16_t port, int listenq)
 * @brief listenq <= 0 active connect listenq > 0 passive connect
 */
ZAPI int zconnectx(zsock_t sock, const char *host, uint16_t port, int listenq, int timeout_ms);

zinline zerr_t zrecv(zsock_t sock, char *buf, int len, int flags){
    zerr_t ret;
    int readed;
    if(0 > (readed = recv(sock, buf, len, flags))){
#ifdef ZSYS_WINDOWS
        ret = WSAGetLastError();
        if(WSAEWOULDBLOCK == ret || WSAEINTR == ret){
            ret = ZEAGAIN;
        }
#else
        ret = errno;
        if(EAGAIN == ret || EWOULDBLOCK == ret || EINTR == ret){
            ret = ZEAGAIN;
        }
#endif
        if(ret != ZEAGAIN){
            zerrno(ret);
            ret = ZEFAIL;
        }
    }else{
        ret = readed;
#if ZTRACE_SOCKET
        /* ztrace_bin(buf, readed); */
        //zdbg("recv: %d", readed);
#endif
    }
    return(ret);
}

/**
 * @brief send <len> number bytes of <buf>
 * @param sock  [in]  socket create by zsocket()
 * @param buf   [in]  send buffer
 * @param len   [in]  buffer length
 *              [out] actually sended bytes
 * @param flags [in]  send flags
 * @return error code
 * @retval ZEOK send buffer success
 * @retval ZEFAIL send buffer fail
 */
zinline zerr_t zsend(zsock_t sock, const char *buf, int *len, int flags){
    zerr_t ret;
    int sended;
    int length = *len;
    ret = ZEOK;
    sended = 0;
    while(sended != length){
#if ZTRACE_SOCKET
        /* ztrace_bin(buf + sended, length - sended); */
#endif
        ret = send(sock, buf + sended, length - sended, flags);

        if(ret >= 0){
            sended += ret;
#if ZTRACE_SOCKET
            //zdbg("send<sended:%d, cur:%d, total:%d>", sended, ret, length);
#endif
        }else{
#ifdef ZSYS_WINDOWS
            ret = WSAGetLastError();
            if(WSAEWOULDBLOCK == ret || WSAEINTR == ret){
                ret = ZEAGAIN;
            }
#else
            ret = errno;
            if(EAGAIN == ret || EWOULDBLOCK == ret || EINTR == ret){
                ret = ZEAGAIN;
            }
#endif
            if(ZEAGAIN == ret){
#ifdef ZTRACE_SOCKET
                //zdbg("try again...");
#endif
                continue;
            }else{
                *len = sended;
                zerrno(ret);
                ret = ZEFAIL;
                break;
            }
        }
    }
    return(ret);
}

#define ZSOCK_CLOSE(sock) do{zsockclose(sock); (sock)=ZINVALID_SOCKET;}while(0)

ZC_END

#endif /*_ZCOM_SOCKET_H_*/
