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
#ifndef _ZCOMMON_DEFINES_H_
#define _ZCOMMON_DEFINES_H_

/**
 * @file defines.h
 * @brief Defines common structures
 * @author Z.Riemann https://github.com/ZRiemann/
 * @date 2018-04-13 Z.Riemann found
 */

/**
 * @brief common identification structure
 * @par session_id, node_id
 */
typedef struct znt_identification_s{
    char *id; /** identification data */
    int len; /** identification data length*/
    int state; /** identify objects state */
}znt_id_t;
typedef znt_id_t znt_sid_t; /** session identification */
typedef znt_id_t znt_nid_t; /** node identification */


#endif /*_ZCOMMON_DEFINES_H_*/
