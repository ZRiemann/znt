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
 * @file tst_state_threads.c
 * @brief <A brief description of what this file is.>
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-04 Z.Riemann found
 *
 * @zmake.app znt;
 *
 * @par passive
 *      thread module: 1 accept thread, N connection thread;
 * @par active
 *      thread module: N connection thread;
 * @par statistic
 *      - global
 *        enable on|off
 *        connections
 *        total sended
 *        total received
 *        Kbps per N seconds
 *      - connection
 *        online timestamp
 *        sended Bytes
 *        received Bytes
 *        Kbps per N seconds
 */
#include <string.h>

#include <zsi/base/error.h>
#include <zsi/base/type.h>
#include <zsi/base/trace.h>
#include <zsi/stl/list.h>
#include <zsi/stl/rbtree.h>
#include <zsi/app/trace2file.h>

#include <znt/common/defines.h>
#include <znt/com/state_threads.h>

typedef struct zst_config_s{
    /* configuration */
    zbool_t is_single; /** ztrue:single process; zfalse:multi process */
    zbool_t is_pool; /** ztrue:pool-accept; zfalse:accept-new-thread */
    zbool_t is_passive; /** ztrue:passive-connection; zfalse:active-connection */
    uint16_t port; /** server port */
    char ip[64]; /** ip string */
    int max_conns; /** maximum connections */
    int bw_interval; /** bandwidth calculate interval (sec) */
    /* statistic */
    int conns; /** current connections */
    /* bandwidth */
    uint64_t read; /** current readed bytes */
    uint64_t writ; /** current writed bytes  */
    uint64_t bw_read; /** old readed bytes */
    uint64_t bw_write; /** old writed bytes */
    time_t bw_timestamp; /** old bandwidth timestamp */
    /* Session Manager */
    uint32_t serial; /** */
    zbtree_t *ssns; /** sessions, connections with id */
}zst_cfg_t;

typedef struct zst_session_s{
    znt_sid_t sid; /** session identification */
    zst_cfg_t *cfg; /** pointer to global st configure */
    time_t conn; /** connection established time */
    time_t timestamp; /** bandwidth timestamp */
    uint64_t read; /** read bytes */
    uint64_t writ; /** write bytes */
    uint64_t old_read; /** read-bandwidth = (readed - old_readed)/(now - timestamp) */
    uint64_t old_writ; /** write-bandwidth = (writed - old_writed)/(now - timestamp) */
}zst_ssn_t;

static zerr_t tc_parse_agr(zst_cfg_t *cfg, int argc, char **argv);
static zerr_t tc_passive(zst_cfg_t *cfg);
static zerr_t tc_active(zst_cfg_t *cfg);
static zerr_t pre_stinit(zop_arg);
static zerr_t post_stinit(zop_arg);
static zerr_t init_config(zst_cfg_t *cfg);
static zerr_t print_config(zst_cfg_t *cfg);

zerr_t tc_state_threads(int argc, char **argv){
    zst_cfg_t cfg = {0};
    zerr_t ret = ZEOK;

    init_config(&cfg);
    if(ZEOK != (ret = tc_parse_agr(&cfg, argc, argv))){
        zerrno(ret);
        return ret;
    }

    zst_init(pre_stinit, post_stinit);

    if(cfg.is_single){
        if(cfg.is_passive){
            tc_passive(&cfg);
        }else{
            tc_active(&cfg);
        }
    }else{
        ret = ZENOT_SUPPORT;
        zerrno(ret);
    }
    return ret;
}

#define ASSERT_STATE(x) if(0 != x){ret = ZEPARAM_INVALID; break;}
static zerr_t tc_parse_agr(zst_cfg_t *cfg, int argc, char **argv){
    int i = 0;
    int state = 0;
    zerr_t ret = ZEOK;
    for(i = 1; i < argc; ++i){
        if(0 == strcmp("--st-single-process", argv[i])){
            ASSERT_STATE(state);
            cfg->is_single = ztrue;
        }else if(0 == strcmp("--st-multi-process", argv[i])){
            ASSERT_STATE(state);
            cfg->is_single = zfalse;
        }else if(0 == strcmp("--st-thread-pool", argv[i])){
            ASSERT_STATE(state);
            cfg->is_pool = ztrue;
        }else if(0 == strcmp("--st-connection-per-thread", argv[i])){
            ASSERT_STATE(state);
            cfg->is_pool = zfalse;
        }else if(0 == strcmp("--ip", argv[i])){
            ASSERT_STATE(state);
            state = 1;
        }else if(0 == strcmp("--port", argv[i])){
            ASSERT_STATE(state);
            state = 2;
        }else if(0 == strcmp("--passive", argv[i])){
            ASSERT_STATE(state);
            cfg->is_passive=ztrue;
        }else{
            if(0 == state){
                zerrno(ZENOT_SUPPORT);
            }else if(1 == state){
                /* read ip parameter */
                strcpy(cfg->ip, argv[i]);
                state = 0;
            }else if(2 == state){
                /* read port parameter */
                cfg->port = atoi(argv[i]);
                state = 0;
            }else{
                zerrno(ZENOT_SUPPORT);
                state = 0;
            }

        }
    }

    print_config(cfg);
    zerrno(ret);
    return ret;
}

static zerr_t post_stinit(zop_arg){
    st_timecache_set(1);
    return ZEOK;
}
static zerr_t pre_stinit(zop_arg){
    st_set_eventsys(ST_EVENTSYS_POLL);
    return ZEOK;
}

static zerr_t init_config(zst_cfg_t *cfg){
    cfg->max_conns = 1024;
    cfg->bw_interval = 5;
    return ZEOK;
}
static zerr_t print_config(zst_cfg_t *cfg){
    ztrace_org("cfg{\n"
               "\tis_single\t%d;\n"
               "\tis_pool\t%d;\n"
               "\tis_passive\t%d;\n"
               "\tport\t%d;\n"
               "\tip\t%s;\n"
               "\tmax_conns\t%d;\n"
               "\tbw_interval\t%d;\n"
               "}\n",
               cfg->is_single,
               cfg->is_pool,
               cfg->is_passive,
               cfg->port,
               cfg->ip,
               cfg->max_conns,
               cfg->bw_interval
        );
    return ZEOK;
}

/******************************************************************************
 * passive connection
 * accept thread create session threads
 */

zptr_t zproc_accept(zptr_t arg){
    
}
zptr_t zproc_session(zptr_t arg){
    
}
static zerr_t tc_passive(zst_cfg_t *cfg){
    /* create and bind listening sockets
     * znt_passive(cfg->ip, (unint16_t)cfg->port);
     */
    zsock_t sock = zsocket(AF_INET, SOCK_STREAM, 0);
    st_netfd_t stfd = NULL;
    zconnectx(sock, cfg->ip, (uint16_t)cfg->port, 500, 3000);

    stfd = zst_socket(sock);
    zst_thread_create();

    zerrno(ZENOT_SUPPORT);
    return ZEOK;
}

/******************************************************************************
 * active connection
 */
static zerr_t tc_active(zst_cfg_t *cfg){
    /* connect remote endpoint */
    zerrno(ZENOT_SUPPORT);
    return ZEOK;
}
