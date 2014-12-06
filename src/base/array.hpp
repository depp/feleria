/* Copyright 2013-2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_ARRAY_HPP
#define LD_BASE_ARRAY_HPP
#include "log.hpp"
#include "sg/opengl.h"
#include <limits>
#include <new>
#include <cstdlib>
namespace Base {

// OpenGL attribute array class.
template<class T>
class Array {
private:
    T *m_data;
    unsigned m_count;
    unsigned m_alloc;

public:
    explicit Array();
    Array(const Array &other) = delete;
    Array(Array &&other);
    ~Array();
    Array &operator=(const Array &other) = delete;
    Array &operator=(Array &&other);

    /// Get the number of elements in the array.
    unsigned size() const;
    /// Determine whether the array is empty.
    bool empty() const;
    /// Set the number of elements in the array to zero.
    void clear();
    /// Reserve space for the given total number of elements.
    void reserve(std::size_t total);
    /// Insert the given number of elements and return a pointer to the first.
    T *insert(std::size_t count);
    /// Set the end of the array.  Must be within the array, or at the end.
    void set_end(T* end);
    /// Upload the array to an OpenGL buffer.
    void upload(GLenum usage);
};

template<class T>
inline Array<T>::Array()
    : m_data(nullptr), m_count(0), m_alloc(0)
{ }

template<class T>
inline Array<T>::Array(Array<T> &&other)
    : m_data(nullptr), m_count(0), m_alloc(0) {
    m_data = other.m_data;
    m_count = other.m_count;
    m_alloc = other.m_alloc;
    other.m_data = nullptr;
    other.m_count = 0;
    other.m_alloc = 0;
}

template<class T>
inline Array<T>::~Array() {
    std::free(m_data);
}

template<class T>
Array<T> &Array<T>::operator=(Array<T> &&other) {
    if (this == &other)
        return *this;
    std::free(m_data);
    m_data = other.m_data;
    m_count = other.m_count;
    m_alloc = other.m_alloc;
    other.m_data = nullptr;
    other.m_count = 0;
    other.m_alloc = 0;
    return *this;
}

template<class T>
unsigned Array<T>::size() const {
    return m_count;
}

template<class T>
bool Array<T>::empty() const {
    return m_count == 0;
}

template<class T>
void Array<T>::clear() {
    m_count = 0;
}

template<class T>
void Array<T>::reserve(std::size_t total) {
    std::size_t rounded;
    if (total > std::numeric_limits<unsigned>::max())
        Log::abort("out of memory");
    if (static_cast<size_t>(m_alloc) >= total)
        return;
    rounded = total - 1;
    rounded |= rounded >> 1;
    rounded |= rounded >> 2;
    rounded |= rounded >> 4;
    rounded |= rounded >> 8;
    rounded |= rounded >> 16;
    rounded += 1;
    if (rounded >= (unsigned) -1)
        Log::abort("out of memory");
    T *newdata = static_cast<T *>(std::realloc(m_data, sizeof(T) * rounded));
    if (!newdata)
        Log::abort("out of memory");
    m_data = newdata;
    m_alloc = (unsigned) rounded;
}

template<class T>
T *Array<T>::insert(std::size_t count) {
    if (count > m_alloc - m_count) {
        if (count > std::numeric_limits<unsigned>::max() - m_count)
            Log::abort("out of memory");
        reserve(count + m_count);
    }
    int pos = m_count;
    m_count += count;
    return m_data + pos;
}

template<class T>
void Array<T>::set_end(T* end) {
    m_count = static_cast<int>(end - m_data);
}

template<class T>
void Array<T>::upload(GLenum usage) {
    glBufferData(GL_ARRAY_BUFFER, m_count * sizeof(T), m_data, usage);
}

}
#endif
