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
#ifndef _ZTST_SOCKET_H_
#define _ZTST_SOCKET_H_

/**
 * @file tst_socket.h
 * @brief <A brief description of what this file is.>
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-03 Z.Riemann found
 *
 * @par throughput
 *      - socket 127.0.0.1 9090 conn 1 null 1024 10240
 *        write_cnt:10485760 bytes_per_write:1024 write_per_sec:1364090.02
 *      - socket 127.0.0.1 9090 listen 1 null 10240
 *        sec:7 usec:687442 bytes:10737418240
 *        bpus:11173 kbps:10911132.812 mpbs:10655.403 gbps:10.406
 *        Bpus:1396 KBps:1363891.602 MBps:1331.925 GBps:1.301
 *        read_count:158906 bytes_per_read:67570 read_per_sec:20672.04
 *
 *
 *      - socket 127.0.0.1 9090 conn 1 null 819200 10240
 *        write_cnt:13108 bytes_per_write:819200 write_per_sec:7426.63
 *      - socket 127.0.0.1 9090 listen 1 null 10240
 *        read_count:24417 bytes_per_read:439777 read_per_sec:13849.69
 *        sec:1 usec:763606 bytes:10738040016
 *        bpus:48709 kbps:47567382.812 mpbs:46452.522 gbps:45.364
 *        Bpus:6088 KBps:5945922.852 MBps:5806.565 GBps:5.670
 */
#include <zsi/base/type.h>

zerr_t tu_socket(zop_arg);
zerr_t tc_socket(zop_arg);

#endif /*_ZTST_SOCKET_H_*/
