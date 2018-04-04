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
#ifndef _ZCOM_ST_H_
#define _ZCOM_ST_H_

/**
 * @file zstate_threads.h
 * @brief Wrap State-Threads functions
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-04 Z.Riemann found
 *
 * @par Types
 *      st_thread_t ; The thread identifier
 *      st_cond_t   ; Condition variable, no need to lock a mutex befor waiting.
 *      st_mutex_t  ; Only be used for intra-process thread sync.
 *      st_utime_t  ; High resolution time (micro)
 *      st_netfd_t  ; fd (socket,pipe,FIFO etc.)
 *      st_switch_cb_t ; Context switch callback function.
 * @par Error Handling
 *      void st_*()
 *      non-void st_*() success: non-negative integer or not-null pointer
 *                      failure: -1 / NULL, and set global errno. perror(3) can be used.
 *
 * @par Library Initialization
 *      st_init() ; Initializes the library runtime.
 *      st_getfdlimit() ; Maximum number of file descriptors process can open
 *      st_set_eventsys() ; Set event notification mechanism, ST_EVENTSYS_DEFAULT/SELECT
 *      st_get_eventsys() ; ST_EVENTSYS_SELECT/POLL/ALT <USE_POLL>
 *      st_get_eventsys_name() ; "select","pool","kqueue"
 *      st_set_switch_in_cb()  ; <resum> Set the optional callback function for thread switches.
 *      st_set_switch_out_cb() ; <pause> NULL to disable the callback, <ST_SWITCH_CB>
 *      st_thread_self() ; <thread_specific_data> to differentiate between threads.
 *
 * @par Thread Control and Identification
 *      st_thread_create()
 *      st_thread_exit() ; When the last thread terminates the process exits with a zero status value.
 *      st_thread_join()
 *      st_thread_self()
 *      st_thread_interrupt() ; interrupts a target thread, delivered only when thread is about to block.
 *      st_sleep() ; return: 0-ok, EINTR-interrupt
 *      st_usleep() ; param: 0/ST_UTIME_NO_WAIT-yields; -1/ST_UTIME_NO_TIMEOUT-only be interrupt.
 *      st_randomize_stacks() ; MAY improve cache performance. only threads created afterward.
 *
 * @par Pre-Thread Private Data
 *      st_key_create()
 *      st_key_getlimit()
 *      st_thread_setspecific() ; Different threads may bind different values to the same key.
 *      st_thread_getspecific()
 *
 * @par Synchronization
 *      st_cond_new()
 *      st_cond_destroy()
 *      st_cond_wait()
 *      st_cond_timedwait()
 *      st_cond_signal()
 *      st_cond_broadcast()
 *
 *      st_mutex_new()
 *      st_mutex_destroy()
 *      st_mutex_lock()
 *      st_mutex_unlock()
 *      st_mutex_unlick()
 *
 * @par Timing
 *      st_utime()
 *      st_set_utime_function() ; replace the default implementation of the st_utime(), call BEFORE st_init()
 *      st_timecache_set() ; cache time
 *      st_time() ; if caching was enabled by st_timecache_set() return cached result else just call time(2)
 *
 * @par I/O Functions
 *      * Timeouts are measured <since the last context switch>
 *      st_netfd_open() ; set non-blocking. NOT passing fd to normal IO functions, instead of st_*()
 *      st_netfd_open_socket() ; slightly more efficient than st_netfd_open(), only be used on SOCKETS
 *      st_netfd_free() ; free a file descriptor object WITHOUT CLOSING the underlying OS file descriptor.
 *      st_netfd_close() ; free and closes the underlying OS file descriptor.
 *      st_netfd_fileno() ; returns an underlying OS file descriptor.
 *      st_netfd_setspecific() ;
 *      st_netfd_getspecific()
 *      st_netfd_serialize_accept() ; called before  multiple server process via for(2)
 *      st_netfd_poll()
 *
 *      st_accept()
 *      st_connect()
 *      st_read()
 *      st_read_fully() ; read full
 *      st_read_resid() ; read full, and allows the caller to know how many bytes were transferred until error 
 *      st_readv()
 *      st_readv_resid()
 *      st_write()
 *      st_write_resid()
 *      st_writev()
 *      st_writev_resid()
 *      st_recvfrom()
 *      st_sendto()
 *      st_recvmsg()
 *      st_sendmsg()
 *      st_open()
 *      st_poll()
 *
 * @par Program Structure
 *      @see zst_init()
 *
 * @par Pool pattern
 *      - One connection per thread.
 *      - Passive connection
 *        + one accept thread, multi-connection thread;
 *          thr<conn0>...thr<connN>thr<accept>
 *          if(accept-ok){
 *            the accept thread become conn thread;
 *            create another accept thread;
 *          }
 *      - Active connection
 */

#include <zsi/base/type.h>

#ifdef ZSYS_POSIX
/*
 * State-Threads only support UNIX like system
 */
#include <errno.h>

#include <st.h>

#include <zsi/base/type.h>
#include <zsi/base/error.h>
#include <zsi/base/trace.h>

zinline zerr_t zst_init(zoperate pre_init, zoperate post_init){
    /*
     * Configure the library by calling these pre-init functions, if desired.
     *
     * st_set_utime_function();
     * st_set_eventsys(ST_EVENTSYS_DEFAULT);
     */
    if(pre_init){
        pre_init(zop_null);
    }
    if(-1 == zst_init()){
        zerrno(errno);
        return ZEFAIL;
    }
    /*
     * Configure the library by calling this post-init functions, if desired.
     *
     * st_timecache_set(on/off)
     * st_randomize_stacks()
     * st_set_switch_in_cb()
     * st_set_switch_out_cb()
     */
    if(post_init){
        post_init(zop_null);
    }
    /*
     * Create resources that will be shared among different processes:
     * - create and bind listening sockets
     * - create shared memory segments
     * - inter-process communication (IPC)
     * - synchronization primitives (if any)
     */
    /*
     * Create several process var fork(2). The parent process should either exit or
     * become a "watchdog"
     *
     * In each child process create a pool of threads to handle user connections.
     * Each thread in the poll may accept client connections, or connect to other servers,
     * perform various network IO
     */

    zdbg("\nfd_limit: %d"
         "\nst_eventsys: %s"
         "\nst_key_limit: %d",
         st_getfdlimit(),
         st_get_eventsys_name(),
         st_key_getlimit());
}

zinline st_thread_t zst_thread_create(void*(*start)(void*), void *arg,
                                      zbool_t joinable, int stack_size){
    st_thread_t thr = st_thread_create(start, arg, joinable, stack_size);
    if(!thr){
        zerrno(errno);
    }else{
        zdbg("create st_thread<ptr:%p self:%p stack_size:%d>",
             thr, st_thread_self(), stack_size);
    }
    return thr;
}

zinline zerr_t zst_thread_join(st_thread_t thread){
    zptr_t ret = NULL;
    if(0 == st_thread_join(thread, &ret)){
        zdbg("join st_thread<ptr:%p, ret:%p>", thread, ret);
    }else{
        /*  EINVAL  Target thread is unjoinable.
         *  EINVAL  Other thread already waits on the same joinable thread. 
         *  EDEADLK  Target thread is the same as the calling thread. 
         *  EINTR  Current thread was interrupted by st_thread_interrupt(). 
         */
        zerrno(errno);
        return ZEFAIL;
    }
    return ZEOK;
}

#endif /* ZSYS_POSIX */
#endif /*_ZCOM_ST_H_*/
