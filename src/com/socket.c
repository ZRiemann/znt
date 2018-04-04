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
/**
 * @file socket.c
 * @brief <A brief description of what this file is.>
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-03 Z.Riemann found
 *
 * @zmake.app znt;
 */
#include <zsi/base/error.h>
#include <zsi/base/trace.h>
#include <znt/com/socket.h>
#ifdef ZSYS_POSIX
#include <arpa/inet.h>
#else
#pragma comment(lib, "Ws2_32")
#endif
#include <string.h>
#include <fcntl.h>
#include <stdio.h>

zerr_t zsock_init(int v1, int v2){
#ifdef ZSYS_WINDOWS
    WORD wVersionRequested;
    WSADATA wsaData;
    zerr_t err;

    wVersionRequested = MAKEWORD( v1, v2 );

    err = WSAStartup( wVersionRequested, &wsaData );
    if ( err != 0 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        return ZEFAIL;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if ( LOBYTE( wsaData.wVersion ) != v1 ||
         HIBYTE( wsaData.wVersion ) != v2 ) {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        WSACleanup( );
        return ZEFAIL;
    }

    /* The WinSock DLL is acceptable. Proceed. */
#endif
    return ZEOK;
}
zerr_t zsock_fini(){
#ifdef ZSYS_WINDOWS
    WSACleanup();
#endif
    return ZEOK;
}

zsock_t zsocket(int domain, int type, int protocol){
    zsock_t sock;
    sock = socket(domain, type, protocol);
    if(ZINVALID_SOCKET == sock){
        zerr_t err;
#ifdef ZSYS_WINDOWS
        err = WSAGetLastError();
#else
        err = errno;
#endif
        zerrno(err);
    }else{
#if ZTRACE_SOCKET
        zdbg("socket<%d>(domain<%d>,type<%d>,protocol<%d>", sock, domain, type, protocol);
#endif
        zsock_nonblock(sock, ztrue); // set default no block
    }
    return(sock);
}

zerr_t zsockclose(zsock_t sock){
    zerr_t ret;
#ifdef ZSYS_WINDOWS
    ret = closesocket(sock);
    ret = (SOCKET_ERROR == ret) ? WSAGetLastError() : ZEOK;
#else //ZSYS_POSIX
    ret = close(sock);
    ret = (ret < 0)?errno:ZEOK;
#endif
    zdbg("close socket<fd:%d> %s", sock, zstrerr(ret));
    return(ret);
}

zerr_t zinet_addr(zsockaddr_in *addr, const char *host, uint16_t port){
    zerr_t ret;

    ret = ZEOK;
    if(!addr || !host){
        return ZEPARAM_INVALID;
    }
    memset(addr, 0, sizeof(zsockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
#ifdef ZSYS_WINDOWS
    if(INADDR_NONE == (addr->sin_addr.S_un.S_addr = inet_addr(host))){
        ret = ZEFAIL;
    }
#else
    if(inet_pton(AF_INET, host, &addr->sin_addr) <=0){
        ret = ZEFAIL;
    }
#endif

#if ZTRACE_SOCKET
    zdbg("zinet(addr<%p>, host<%s>, port<%d>); %s", addr, host, port, zstrerr(ret));
#endif

    zerrno(ret);
    return(ret);
}

zerr_t zinet_str(zsockaddr_in *addr, char *host, uint16_t *port){
    /* Assert */
    *port = addr->sin_port;
#ifdef ZSYS_WINDOWS
    sprintf(host, "notsupport");
#else
    inet_ntop(AF_INET, addr, host, sizeof(zsockaddr_in));
#endif
    return ZEOK;
}

zerr_t zgetpeername(zsock_t sock, zsockaddr_in *addr, char *host, uint16_t *port){
    zerr_t ret = ZEOK;
#ifdef ZSYS_WINDOWS
    zerrno(ZNOT_SUPPORT);
#else
    socklen_t len = sizeof(zsockaddr_in);
    if(!(ret = getpeername(sock, (ZSA*)addr, &len)))
    {
        if(host && port){
            sprintf(host, "%s", inet_ntoa(addr->sin_addr));
            *port = ntohs(addr->sin_port);
        }
    }else{
        ret = errno;
    }
#endif
    zerrno(ret);
    return ret;
}
zerr_t zconnect(zsock_t sock, const ZSA *addr, int len){
    zerr_t ret;

    ret = ZEOK;
    if(connect(sock, addr, len) < 0){
#ifdef ZSYS_WINDOWS
        ret = WSAGetLastError();
#else
        ret = errno;
#endif
        zerrno(ret);
        ret = ZEFAIL;
    }
#if ZTRACE_SOCKET
    else{
        zerrno(ret);
    }
#endif
    return(ret);
}

#if 0
zerr_t zrecv(zsock_t sock, char *buf, int len, int flags){
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
        ztrace_bin(buf, readed);
#endif
    }
    return(ret);
}

zerr_t zsend(zsock_t sock, const char *buf, int len, int flags){
    zerr_t ret;
    int sended;
    ret = 0;
    sended = 0;
    while(sended != len){
#if ZTRACE_SOCKET
        ztrace_bin(buf + sended, len - sended);
#endif
        ret = send(sock, buf+sended, len-sended, flags);

        if(ret >= 0){
            sended += ret;
#if ZTRACE_SOCKET
            zdbg("send<sended:%d, cur:%d, total:%d>", sended, ret, len);
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
                zdbg("try again...");
#endif
                continue;
            }else{
                zerrno(ret);
                ret = ZEFAIL;
                break;
            }
        }
    }
    if(ret != ZEFAIL){
        ret = sended;
    }
    return(ret);
}
#endif // if 0

zerr_t zrecv_packet(zsock_t sock, char *buf, int maxlen, int *offset, int *len, char bitmask){
    zerr_t ret = ZEFAIL;
    /* remove old data and move new data to header */
    if(*offset != 0){
        if(*offset != *len){
            while(*(buf+(*offset)) != bitmask){
                /* zdbg("drop<%02x>", *(buf+(*offset))); */
                ++(*offset);
                if(*offset == *len){
                    *offset = *len = 0;
                    break;
                }
            }
            memmove(buf, buf + *offset, *len - *offset);
            *len = *len - *offset;
            *offset = 0;

            /* parse next packet */
            if(*len != *offset){
                while(++(*offset)){
                    if(*offset == *len){
                        *offset = *len = 0;
                        break;
                    }
                    if(*(buf+(*offset)) == bitmask){
                        ++(*offset);
                        ret = *offset;
                        return(ret);
                    }
                }
            }
        }else{
            *offset = *len = 0;
        }
    }

    ret = zrecv(sock, buf+(*len), maxlen-(*len), 0);
    if(ZEAGAIN != ret && ZEFAIL != ret && 0 != ret){
        *len += ret;
        while(++(*offset)){
            if(*(buf+(*offset)) == bitmask){
                ++(*offset);
                ret = *offset;
                break;
            }
            if(*len == *offset){
                ret = ZEAGAIN;
                break;
            }
        }
    }
    return(ret);
}

zerr_t zselect(int maxfdp1, fd_set *read, fd_set *write, fd_set *except, struct timeval *timeout){
    zerr_t ret;
    ret = select(maxfdp1, read, write, except, timeout);
    if(ret < 0){
#ifdef ZSYS_WINDOWS
        ret = WSAGetLastError();
#else
        ret = errno;
#endif
        zerrno(ret);
        ret = ZEFAIL;
    }
    return(ret);
}

zerr_t zsock_nonblock(zsock_t sock, int noblock){
    zerr_t ret = ZEOK;
#ifdef ZSYS_WINDOWS
    unsigned long ul = noblock;
    ret = ioctlsocket(sock, FIONBIO, &ul);
    if(SOCKET_ERROR == ret){
        ret = WSAGetLastError();
        zerrno(ret);
        ret = ZEFAIL;
    }
#else
    int val;
    val = fcntl(sock, F_GETFL, 0);
    if(1 == noblock){
        val |= O_NONBLOCK;
    }else{
        val &= (~O_NONBLOCK);
    }
    fcntl(sock, F_SETFL, val);
#endif
    return(ret);
}

zerr_t zbind(zsock_t sock, const ZSA *addr, int len){
    zerr_t ret;
    ret = bind(sock, addr, len);
#ifdef ZSYS_WINDOWS
    ret = (SOCKET_ERROR == ret) ? WSAGetLastError() : ZEOK;
#else
    ret = (ret < 0) ? errno : ZEOK;
#endif
    zerrno(ret);
    return(ret);
}

zerr_t zlisten(zsock_t sock, int listenq){
    zerr_t ret;
    ret = listen(sock, listenq);
#ifdef ZSYS_WINDOWS
    ret = (SOCKET_ERROR == ret) ? WSAGetLastError() : ZEOK;
#else
    ret = (ret < 0) ? errno : ZEOK;
#endif
    zerrno(ret);
    return(ret);
}

zsock_t zaccept(zsock_t sock, ZSA *addr, int *addrlen){
    zerr_t ret;
    zsock_t sk;
#ifdef ZSYS_WINDOWS
    sk = accept(sock, addr, addrlen);
    ret = (INVALID_SOCKET == sk) ? WSAGetLastError() : ZEOK;
#else
    sk = accept(sock, addr, (socklen_t*)addrlen);
    ret = (sk < 0) ? errno : ZEOK;
#endif
    zdbg("accept <fd:%d> %s", sk, zstrerr(ret));
    return(sk);
}

zerr_t zconnectx(zsock_t sock, const char *host, uint16_t port, int listenq, int timeout_ms){
    zerr_t ret;
    zsockaddr_in addr;

    ret = zinet_addr(&addr, host, port);
    if(ret != ZEOK){
        return(ret);
    }

    if(listenq > 0){
        int reuse = 1;
#ifdef ZSYS_WINDOWS
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse));
#else
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
        zbind(sock, (ZSA*)&addr, sizeof(addr));
        ret = zlisten(sock, listenq);
#if ZTRACE_SOCKET
        zdbg("sock<%d> bind and listen<que:%d, addr:%s:%d> by reuse address.", sock, listenq, host ? host : "ADDR_ANY", port);
#endif
    }else{
        if(-1 == timeout_ms){
            // block connect
#if ZTRACE_SOCKET
            zdbg("sock<%d> start block connect<%s:%d>...", sock, host, port);
#endif
            zsock_nonblock(sock, 0);
            ret = zconnect(sock, (ZSA*)&addr, sizeof(addr));
            zsock_nonblock(sock, 1);
#if ZTRACE_SOCKET
            zdbg("block connect down.");
#endif
            return ret;
        }
        // nonblock connect
#if ZTRACE_SOCKET
        zdbg("slock<%d> start nonblock connect<%s:%d>...", sock, host, port);
#endif
        if(ZEOK != (ret = zconnect(sock, (ZSA*)&addr, sizeof(addr)))){
            struct timeval tv;
            fd_set rset, wset;
            int error;
            socklen_t len;
            if(timeout_ms < 1000 || timeout_ms > 15000){
                timeout_ms = 4000;
            }
            tv.tv_sec = timeout_ms/1000;
            tv.tv_usec = (timeout_ms%1000)*1000;
            FD_ZERO(&rset);
            FD_SET(sock, &rset);
            FD_ZERO(&wset);
            FD_SET(sock, &wset);
#if ZTRACE_SOCKET
            zdbg("conect timeout<sec:%d, usec:%d>", tv.tv_sec, tv.tv_usec);
#endif
            if((ret = zselect(sock+1, &rset, &wset, NULL, &tv)) > 0){
                zdbg("select<%d>", ret);
                len = sizeof(error);
#ifdef ZSYS_WINDOWS
                getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&error,&len);
#else
                getsockopt(sock, SOL_SOCKET, SO_ERROR, &error,&len);
#endif
                if(0 == error){
                    ret = ZEOK;
                    zdbg("connect ok");
                }else{
                    ret = ZEFAIL;
                    zdbg("connect fail");
                }
            }else{
                ret = ZETIMEOUT;
            }
        }
    }
    zerrno(ret);
    return(ret);
}
