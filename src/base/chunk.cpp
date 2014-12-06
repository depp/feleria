/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "chunk.hpp"
#include "log.hpp"
#include "sg/defs.h"

#include <cstring>

namespace Base {

namespace {

const char NATIVE_BYTE_ORDER[2] = {
#if SG_BYTE_ORDER == SG_LITTLE_ENDIAN
    'L', 'E'
#else
    'B', 'E'
#endif
};

}

struct ChunkReader::Entry {
    char name[4];
    unsigned offset;
    unsigned length;
};

ChunkReader::ChunkReader()
    : m_first(nullptr), m_last(nullptr) {}

ChunkReader::~ChunkReader() {}

bool ChunkReader::read(const Data &data) {
    const char *ptr = reinterpret_cast<const char *>(data.ptr());
    size_t size = data.size();
    if (size < 24) {
        return false;
    }
    if (std::memcmp(ptr + 18, NATIVE_BYTE_ORDER, 2)) {
        Log::warn("%s: Not native byte order.", data.path());
        return false;
    }
    unsigned count = *reinterpret_cast<const unsigned *>(ptr + 20);
    if (count > 0x10000 || count * 12 > size - 24) {
        return false;
    }
    const Entry *first = reinterpret_cast<const Entry *>(ptr + 24),
        *last = first + count;
    for (auto i = first; i != last; i++) {
        if (i->offset > size || i->length > size - i->offset) {
            return false;
        }
    }

    m_data = data;
    m_first = first;
    m_last = last;
    return true;
}

const char *ChunkReader::magic() const {
    return reinterpret_cast<const char *>(m_data.ptr());
}

ChunkReader::Version ChunkReader::version() const {
    const unsigned char *ptr =
        reinterpret_cast<const unsigned char *>(m_data.ptr());
    std::size_t size = m_data.size();

    if (size >= 24) {
        return Version(ptr[16], ptr[17]);
    } else {
        return Version(-1, -1);
    }
}

ChunkReader::ChunkData ChunkReader::get(const char *name) const {
    for (auto i = m_first, e = m_last; i != e; i++) {
        if (!std::memcmp(i->name, name, 4)) {
            const char *ptr = reinterpret_cast<const char *>(m_data.ptr());
            return ChunkData(ptr + i->offset, i->length);
        }
    }
    return ChunkData(nullptr, 0);
}

}
