/*
 * Copyright 2011, 2012, 2013 Exavideo LLC.
 * 
 * This file is part of openreplay.
 * 
 * openreplay is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * openreplay is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with openreplay.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _NUMARRAY_COPY_H
#define _NUMARRAY_COPY_H

#include <stddef.h>

template <class T, class U>
void numarray_copy(T *dst, const U *src, size_t n) {
    /* 
     * this works for dst and src overlapping, 
     * as long as the move is downward 
     */
    for (size_t i = 0; i < n; i++) {
        dst[i] = src[i];
    }
}

template <class T, class U>
void numarray_set(T *dst, U src, size_t n) {
    while (n != 0) {
        n--;
        dst[n] = src;
    }
}

#endif
