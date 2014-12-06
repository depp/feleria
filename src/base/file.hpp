/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_FILE_HPP
#define LD_BASE_FILE_HPP
#include "sg/file.h"
#include <cstddef>
#include <string>
namespace Base {

class Data {
    sg_filedata *m_data;

public:
    Data() : m_data(nullptr) { }
    Data(const Data &other) : m_data(other.m_data) {
        if (m_data)
            sg_filedata_incref(m_data);
    }
    Data(Data &&other) : m_data(other.m_data) {
        other.m_data = nullptr;
    }
    ~Data() {
        if (m_data)
            sg_filedata_decref(m_data);
    }
    Data &operator=(const Data &other) {
        if (other.m_data)
            sg_filedata_incref(other.m_data);
        if (m_data)
            sg_filedata_decref(m_data);
        m_data = other.m_data;
        return *this;
    }
    Data &operator=(Data &&other) {
        sg_filedata *tmp = other.m_data;
        other.m_data = nullptr;
        m_data = tmp;
        return *this;
    }

    /// Get the start of the buffer.
    const void *ptr() const { return m_data ? m_data->data : nullptr; }
    /// Get the number of bytes in the buffer.
    std::size_t size() const { return m_data ? m_data->length : 0; }
    /// Get the actual path to the file.
    const char *path() const { return m_data ? m_data->path : nullptr; }
    /// Read the contents of a file.
    void read(const std::string &path, size_t maxsz) {
        read(path, maxsz, nullptr);
    }
    /// Read the contents of a file.
    void read(const std::string &path, size_t maxsz,
              const char *extensions);
};

}
#endif
