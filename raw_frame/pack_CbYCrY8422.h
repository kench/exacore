/*
 * Copyright 2011 Exavideo LLC.
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

#ifndef _PACK_CBYCRY8422_H
#define _PACK_CBYCRY8422_H

#include "raw_frame.h"
#include "cpu_dispatch.h"

void YCbCr8P422_CbYCrY8422_default(size_t, uint8_t *, uint8_t *,
        uint8_t *, uint8_t *);

void YCbCr8P422_CbYCrY8422_A_default(size_t, size_t, size_t, size_t, size_t, 
        uint8_t *, uint8_t *, uint8_t *, uint8_t *);

void YCbCr10P422_CbYCrY8422_A_default(size_t, size_t, size_t, size_t, size_t,
        uint16_t *, uint16_t *, uint16_t *, uint8_t *);

void YCbCr8P420_CbYCrY8422_A_default(size_t, size_t, size_t, size_t, size_t,
    uint8_t *, uint8_t *, uint8_t *, uint8_t *);

#ifndef SKIP_ASSEMBLY_ROUTINES_
    extern "C" void YCbCr8P422_CbYCrY8422_vector(size_t, uint8_t *, uint8_t *,
        uint8_t *, uint8_t *);
    //extern "C" void YCbCr8P422_CbYCrY8422_A_vector(size_t, size_t, size_t, size_t, 
        //size_t, uint8_t *, uint8_t *, uint8_t *, uint8_t *);
#endif

class CbYCrY8422Packer : public RawFramePacker {
    public:
        CbYCrY8422Packer(RawFrame *f) : RawFramePacker(f) {
            if (cpu_sse2_available( )) {
                do_YCbCr8P422 = YCbCr8P422_CbYCrY8422_vector;
                //do_YCbCr8P422A = YCbCr8P422_CbYCrY8422_A_vector;
            } else {
                do_YCbCr8P422 = YCbCr8P422_CbYCrY8422_default;
            }
            do_YCbCr8P422A = YCbCr8P422_CbYCrY8422_A_default;
            do_YCbCr10P422A = YCbCr10P422_CbYCrY8422_A_default;
            do_YCbCr8P420A = YCbCr8P420_CbYCrY8422_A_default;
        }
};

#endif
