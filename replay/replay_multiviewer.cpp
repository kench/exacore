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

#include "replay_multiviewer.h"

ReplayMultiviewer::ReplayMultiviewer(DisplaySurface *dpy_) {
    dpy = dpy_;
}

ReplayMultiviewer::~ReplayMultiviewer( ) {

}

void ReplayMultiviewer::add_source(const SourceParams &params) {
    sources.push_back(params);
}

void ReplayMultiviewer::start( ) {
    start_thread( ); 
}

void ReplayMultiviewer::run_thread( ) {
    for (;;) {
        for (int i = 0; i < sources.length( ); i++) {
            const SourceParams &src = sources[i];
            ReplayRawFrame *f = src.source->get( );
            if (f != NULL) {
                dpy->draw->blit(src.x, src.y, f);
            }
        }
        
        dpy->flip( );
    }
}