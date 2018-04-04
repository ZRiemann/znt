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
 * @file tst_socket.c
 * @brief <A brief description of what this file is.>
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-03 Z.Riemann found
 *
 * @zmake.app znt;
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <zsi/base/error.h>
#include <zsi/base/trace.h>
#include <zsi/base/time.h>
#include <zsi/app/interactive.h>
#include <znt/com/socket.h>

static zerr_t tc_socket_base(zop_arg);
static zerr_t tc_socket_listen(int argc, char **argv);
static zerr_t tc_socket_conn(int argc, char **argv);

zerr_t tu_socket(zop_arg){
    printf("# socket <ip> <port> <conn> <conns> <ehco|pecho|null> <block-size:Byte> <total-size:MB>\n");
    printf("# socket <ip> <port> <listen> <conns> <ehco|null> <statistic-size:MB>\n");
    return ZEOK;
}

zerr_t tc_socket(zop_arg){
    zerr_t ret = ZEOK;
    char **argv = ((zitac_arg_t *)in)->argv;
    int argc = ((zitac_arg_t *)in)->argc;
    zsock_t sock = ZINVALID_SOCKET;

    zsock_init(2, 2);

    if(1 == argc){
        ret = tc_socket_base(in, out, hint);
    }else if(7 == argc &&
             0 == strcmp("listen", argv[3])){
        tc_socket_listen(argc, argv);
    }else if(8 == argc &&
             0 == strcmp("conn", argv[3])){
        tc_socket_conn(argc, argv);
    }else{
        /* dump error message and usage */
        printf("socket parameter invalid, as follow usage:\n");
        tu_socket(in, out, hint);
    }

    zsock_fini();
    zerrno(ret);
    return ret;
}

static zerr_t tc_socket_base(zop_arg){
    zerr_t ret = ZEOK;
    /* zitac_arg_t *arg = (zitac_arg_t *)in;*/

    return ret;
}

static zerr_t tc_socket_listen(int argc, char **argv){
    /* # socket <ip> <port> <listen> <conns> <ehco|null> <statistic-size:MB> */
    const BUF_SIZE = 1024 * 1024;
    int port = atoi(argv[2]);
    int conns = atoi(argv[4]);
    uint64_t static_size = atoi(argv[6]);
    char *mode = argv[5];

    static_size *= 1024;
    static_size *= 1024;
    if(0 == static_size){
        static_size = 0x7fffffffffffffff;
    }

    if(1 == conns){
        char *buf = calloc(1, BUF_SIZE);
        int len = BUF_SIZE;
        int nread = 0;
        int read_cnt = 0;
        uint64_t readed = 0;
        zsock_t sock = zsocket(AF_INET, SOCK_STREAM, 0);
        zsock_t conn = ZINVALID_SOCKET;
        ztick_t tick = NULL;
        int sec = 0;
        int usec = 0;
        double throughput = .0;
        double interval = .0;

        zsock_nonblock(sock, zfalse);
        zconnectx(sock, argv[1], (uint16_t)port, 500, 3000);
        conn = zaccept(sock, NULL, NULL);
        if(ZINVALID_SOCKET != conn){
#if 0
            for(;;){
                recv(conn, buf, len, 0);
            }
#else
            while(static_size > readed){
                if((nread = zrecv(conn, buf, len, 0)) > 0){
                    if(0 == readed){
                        /* begin timer */
                        tick = ztick();
                    }
                    readed += nread;
                    ++read_cnt;
                }else{
                    break;
                }
            }
#endif
            /*end timer */
            ztock(tick, &sec, &usec);
            zthrouthput(sec, usec, readed, 0);
            zinf("\nread_count:%d bytes_per_read:%d read_per_sec:%.2f",
                 read_cnt, readed/read_cnt, read_cnt/((double)sec + ((double)(usec/1000))/1000));
        }else{
            zerrno(ZEFAIL);
        }
        ZSOCK_CLOSE(conn);
        ZSOCK_CLOSE(sock);
        free(buf);
    }else{
        zerrno(ZENOT_SUPPORT);
    }
    return ZEOK;
}

static zerr_t tc_socket_conn(int argc, char **argv){
    /* # socket <ip> <port> <conn> <conns> <ehco|pecho|null> <block-size> <total-size:MB> */
    const BUF_SIZE = 1024 * 1024;
    int port = atoi(argv[2]);
    int conns = atoi(argv[4]);
    int block_size = atoi(argv[6]);
    uint64_t total_size = atoi(argv[7]);
    char *mode = argv[5];

    total_size *= 1024;
    total_size *= 1024;
    if(block_size > BUF_SIZE){
        block_size = BUF_SIZE;
    }
    if(0 == total_size){
        total_size = 0x7fffffffffffffff;
    }

    if(1 == conns){
        char *buf = calloc(1, BUF_SIZE);
        int len = BUF_SIZE;
        int write_cnt = 0;
        uint64_t sended = 0;
        zsock_t sock = zsocket(AF_INET, SOCK_STREAM, 0);
        ztick_t tick = NULL;
        int sec = 0;
        int usec = 0;
        double throughput = .0;
        double interval = .0;

        if(ZEOK != zconnectx(sock, argv[1], (uint16_t)port, 0, 3000)){
            ZSOCK_CLOSE(sock);
            free(buf);
            zerrno(ZEFAIL);
            return ZEFAIL;
        }

        zsleepms(200);
        tick = ztick();
#if 0
        for(;;){
            send(sock, buf, block_size, 0);
        }
#else
        while(total_size > sended){
            len = block_size;
            if(ZEFAIL == zsend(sock, buf, &len, 0)){
                sended += len;
                break;
            }
            sended += len;
            ++write_cnt;
        }
#endif
        ztock(tick, &sec, &usec);
        zthrouthput(sec, usec, sended, 0);
        zinf("\nwrite_cnt:%d bytes_per_write:%d write_per_sec:%.2f",
             write_cnt, sended/write_cnt, write_cnt/((double)sec + ((double)(usec/1000))/1000));
        free(buf);
        ZSOCK_CLOSE(sock);
    }else{
        zerrno(ZENOT_SUPPORT);
    }
    return ZEOK;
}
