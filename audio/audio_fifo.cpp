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

#include "numarray.h"
#include <algorithm>
#include <stdexcept>
#include <string.h>

template <class T>
AudioFIFO<T>::AudioFIFO(size_t n_channels) {
    const size_t initial_size = 65536;

    size = initial_size;
    _data = new T[size];
    fill_level = 0;
    this->n_channels = n_channels;
}

template <class T>
AudioFIFO<T>::~AudioFIFO( ) {
    delete [] _data;
}

template <class T> template <class U>
void AudioFIFO<T>::add_packed_samples(const U *idata, size_t n) {
    size_t new_fill_level = fill_level + n * n_channels;

    if (new_fill_level > size) {
        reallocate(std::max(2*size, new_fill_level));
    }

    numarray_copy(&_data[fill_level], idata, n * n_channels);
    fill_level = new_fill_level;
}

template <class T> template <class U>
void AudioFIFO<T>::add_packet(const PackedAudioPacket<U> *apkt) {
    if (apkt->channels( ) != n_channels) {
        /* maybe eventually we can do channel mapping and retry */
        throw std::runtime_error("channel count mismatch!");
    }

    if (fill_level + apkt->size_words( ) > size) {
        reallocate(std::max(2*size, fill_level + apkt->size_words( )));
    }

    numarray_copy(&_data[fill_level], apkt->data( ), apkt->size_words( ));
    fill_level += apkt->size_words( );
}

template <class T> template <class U>
void AudioFIFO<T>::add_packet(const PlanarAudioPacket<U> *apkt) {
    PackedAudioPacket<U> *packed_pkt = apkt->make_packed( );
    add_packet(packed_pkt);
    delete packed_pkt;
}

template <class T>
void AudioFIFO<T>::pop_words(size_t n) {
    if (n > fill_level) {
        throw std::runtime_error("cannot pop that many words");
    }

    numarray_copy(_data, _data + n, fill_level - n);
    fill_level -= n;
}

/* Extract some audio data from the front of the buffer. */
template <class T> template <class U>
void AudioFIFO<T>::peek_packet(PackedAudioPacket<U> *apkt) const {
    if (apkt->channels( ) != n_channels) {
        throw std::runtime_error("peek_packet: apkt has wrong n_channels");
    }

    if (fill_level < apkt->size_words( )) {
        throw std::runtime_error("not enough data in AudioFIFO");
    }

    /* copy out first data from buffer */
    numarray_copy(apkt->data( ), _data, apkt->size_words( ));
}

/* Same as above, but with implicit pop */
template <class T> template <class U>
void AudioFIFO<T>::fill_packet(PackedAudioPacket<U> *apkt) {
    peek_packet(apkt);

    /* move data down and adjust fill level */
    pop_words(apkt->size_words( ));
}


/* Wrappers around the above for planar audio extraction */
template <class T> template <class U>
void AudioFIFO<T>::peek_packet(PlanarAudioPacket<U> *apkt) const {
    /* fill temporary PackedAudioPacket, convert to planar, then copy */
    PackedAudioPacket<U> u(apkt->size_samples( ), apkt->channels( ));
    peek_packet(u);

    PlanarAudioPacket<U> *p = u.make_planar( );
    assert(p->size_words( ) == apkt->size_words( ));
    numarray_copy(apkt->data( ), p->data( ), p->size_words( ));
    delete p;
}

template <class T> template <class U>
void AudioFIFO<T>::fill_packet(PlanarAudioPacket<U> *apkt) {
    peek_packet(apkt);
    pop_words(apkt->size_words( ));
}

template <class T> template <class U>
void AudioFIFO<T>::fill_channel(
    PlanarAudioPacket<U> *apkt,
    unsigned int ch
) {
    U *dp;
    size_t i;

    if (apkt->size_samples( ) > fill_samples( )) {
        throw std::runtime_error("inadequate amount of data in FIFO");
    }
    
    dp = apkt->channel(ch);

    for (i = 0; i < apkt->size_samples( ); i++) {
        *dp = _data[i*n_channels];
        dp++;
    }

    pop_samples(apkt->size_samples( ));

}

template <class T> template <class U>
void AudioFIFO<T>::fill_channel(
    PackedAudioPacket<U> *apkt,
    unsigned int ch
) {
    U *dp;
    size_t i;

    if (apkt->size_samples( ) > fill_samples( )) {
        throw std::runtime_error("inadequate amount of data in FIFO");
    }

    if (ch >= apkt->channels( )) {
        throw std::runtime_error("not enough channels in apkt");
    }

    dp = apkt->data( ) + ch;

    for (i = 0; i < apkt->size_samples( ); i++) {
        *dp = _data[i*n_channels];
        dp += apkt->channels( );
    }

    pop_samples(apkt->size_samples( ));
}

/* Reallocate internal buffer and copy data. */
template <class T>
void AudioFIFO<T>::reallocate(size_t new_size) {
    T *new_data;

    if (new_size > size) {
        new_data = new T[new_size];
        numarray_copy(new_data, _data, fill_level);
        delete [] _data;
        _data = new_data;
        size = new_size;
    }
}


