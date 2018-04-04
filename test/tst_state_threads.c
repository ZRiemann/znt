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
 */
#include <zsi/base/error.h>
#include <zsi/base/type.h>
#include <zsi/base/trace.h>
#include <zsi/app/trace2file.h>

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
}zst_cfg_t;

typedef struct zst_connection_s{
    zst_cfg_t *cfg; /** pointer to global st configure */
    time_t conn; /** connection established time */
    time_t timestamp; /** bandwidth timestamp */
    uint64_t readed; /** read bytes */
    uint64_t writed; /** write bytes */
    uint64_t old_readed; /** read-bandwidth = (readed - old_readed)/(now - timestamp) */
    uint64_t old_writed; /** write-bandwidth = (writed - old_writed)/(now - timestamp) */
}zst_cnn_t;

static zerr_t tc_parse_agr(zst_cfg_t *cfg, int argc, char **argv);
static zerr_t tc_passive(zst_cfg_t *cfg);
static zerr_t tc_active(zst_cfg_t *cfg);
static zerr_t post_init(zop_arg);

zerr_t tc_state_threads(int argc, char **argv){
    zst_cfg_t cfg = {0};
    zerr_t ret = ZEOK;
    if(ZEOK != (ret = tc_parse_agr(&cfg, argc, argv))){
        zerrno(ret);
        return ret;
    }

    zst_init(NULL, post_init);

    if(cfg->is_single){
        if(is_passive){
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

#define ASSERT_STATE(x) if(0 != x){ret = ZEPARAM_INVALID, break;}
static zerr_t tc_parse_agr(zst_cfg_t *cfg, int argc, char **argv){
    int i = 0;
    int state = 0;
    zerr_t ret = ZEOK;
    for(i = 0; i < argc; ++i){
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
    zerrno(ret);
    return ret;
}

static zerr_t tc_passive(zst_cfg_t *cfg){
    /* create and bind listening sockets */
    return ZEOK;
}

static zerr_t tc_active(zst_cfg_t *cfg){
    /* connect remote endpoint */
    return ZEOK;
}

static zerr_t post_init(zop_arg){
    st_timecache_set(1);
    return ZEOK;
}
