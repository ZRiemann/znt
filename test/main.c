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
 * @file main.c
 * @brief node topology layer test main file
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-02 Z.Riemann found
 *
 * @zmake.build on;
 * @zmake.install off;
 * @zmake.link -lst -lzsi -pthread;
 * @zmake.app znt;
 */
#include <stdio.h>
#include <string.h>
#include <zsi/base/error.h>
#include <zsi/base/trace.h>
#include <zsi/app/trace2console.h>
#include <zsi/app/trace2file.h>
#include <zsi/app/interactive.h>

#include "tst_socket.h"
#include "tst_state_threads.h"

static void zprint_help();
static void ztrace2znt(const char *msg, int msg_len, zptr_t hint);
static void zregister_mission(zitac_t itac);

/**
 * @brief main entry
 * @par state_threads testing case
 *      options:
 *       --st-single-process
 *       --st-multi-process
 *       --listen
 *       --ip <dest-ip>
 *       --port <port>
 * @par user interactive
 *      no-options
 */
int main(int argc, char **argv){
    zitac_t itac = NULL;
    ztrace_register(ztrace2console, NULL);

    do{
        if(argc > 1){
            if(strstr(argv[1], "--help")){
                zprint_help();
                break;
            }
            if(strstr(argv[1], "--st")){
                /* state-threads test case */
                tc_state_threads(argc, argv);
                break;
            }
            zprint_help();
            break;
        }
        /* user interactive test case */
        if(!(itac = zitac_init())){
            zerrno(ZEMEM_INSUFFICIENT);
            break;
        }
        zregister_mission(itac);
        zitac_repl(itac);
        zitac_fini(itac);
    }while(0);

    zmsg("test znt exit now.");
    return 0;
}

static void ztrace2znt(const char *msg, int msg_len, zptr_t hint){
    ztrace2console(msg, msg_len, hint);
    ztrace2file(msg, msg_len, hint);
}

#define ZREG_MIS(key) zitac_reg_mission(itac, #key, strlen(#key), tu_##key, tc_##key)
static void zregister_mission(zitac_t itac){
    ZREG_MIS(socket);
}

static void zprint_help(){
    printf("--help\t\t\t\tDisplay this message.\n"
           "State Threads:\n"
           "--st-single-process\t\t\t\tSingle process\n"
           "--st-multi-process\t\t\t\tMulti process\n"
           "--listen\t\t\t\tPassive connection as a SERVER\n"
           "--ip <dest-ip>\t\t\t\tRemote/listen endpoint ip\n"
           "--port <port>\t\t\t\tRemote/Listen endpoint port\n"
        );
}