/*
 * Copyright 2013 Exavideo LLC.
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

#ifndef _REPLAY_FRAME_CACHE_H
#define _REPLAY_FRAME_CACHE_H

#include "raw_frame.h"
#include "replay_buffer.h"
#include "mjpeg_codec.h"
#include "phase_data_packet.h"

/* Cache decoded frames for faster access. */
class ReplayFrameCache {
    public:
        ReplayFrameCache( );
        ~ReplayFrameCache( );
        RawFrame *get_frame(ReplayBuffer *source, timecode_t tc);
        PhaseDataPacket *get_phase_data(ReplayBuffer *source, timecode_t tc);
    protected:
        ReplayFrameData *cached_compressed_frame;
        RawFrame *cached_raw_frame;
        AudioPacket *cached_audio_packet;
        PhaseDataPacket *cached_phase_data;
        Mjpeg422Decoder decoder;
        void check_cache(ReplayBuffer *source, timecode_t tc);
};

#endif
