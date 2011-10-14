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

#ifndef _REPLAY_INGEST_H
#define _REPLAY_INGEST_H

#include "thread.h"
#include "async_port.h"
#include "adapter.h"
#include "replay_data.h"
#include "replay_buffer.h"

class ReplayIngest : public Thread {
    public:
        ReplayIngest(InputAdapter *iadp_, ReplayBuffer *buf_);
        ~ReplayIngest( );

        AsyncPort<ReplayRawFrame> monitor;
        AsyncPort<ReplayRawFrame> *get_monitor( ) { return &monitor; }
    protected:
        void run_thread( );
        
        InputAdapter *iadp;
        ReplayBuffer *buf;

        ReplayIngest() { };
};

#endif

