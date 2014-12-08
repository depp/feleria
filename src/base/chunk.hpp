/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_CHUNK_HPP
#define LD_BASE_CHUNK_HPP
#include "file.hpp"
#include "range.hpp"
#include <utility>
namespace Base {

/// Object for reading chunked file formats.
class ChunkReader {
public:
    typedef std::pair<int, int> Version;
    typedef std::pair<const void *, std::size_t> ChunkData;

private:
    struct Entry;

    Data m_data;
    const Entry *m_first, *m_last;

public:
    ChunkReader();
    ChunkReader(const ChunkReader &) = delete;
    ~ChunkReader();
    ChunkReader &operator=(const ChunkReader &) = delete;

    /// Read a chunked file, returns true if the file is well-formed.
    bool read(const ::Base::Data &data);

    /// Get the file's magic cookie.
    const char *magic() const;

    /// Get the file's version.
    Version version() const;

    /// Get file chunk data.
    ChunkData get(const char *name) const;

    /// Get file chunk data.
    template<class T>
    Range<T> get_array(const char *name) const {
        auto chunk = get(name);
        const T *first = reinterpret_cast<const T *>(chunk.first);
        std::size_t count = chunk.second / sizeof(T);
        return Range<T>(first, first + count);
    }
};

}
#endif
