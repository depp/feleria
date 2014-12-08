/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_BASE_RANGE_HPP
#define LD_BASE_RANGE_HPP
#include <vector>
namespace Base {

/// Range object for iteration.
template<class T>
class Range {
private:
    const T *m_first, *m_last;

public:
    Range() : m_first(nullptr), m_last(nullptr) { }
    Range(const T *first, const T *last) : m_first(first), m_last(last) { }
    explicit Range(const std::vector<T> &vec)
        : m_first(vec.data()), m_last(vec.data() + vec.size()) { }
    const T *begin() const { return m_first; }
    const T *end() const { return m_last; }
    std::size_t size() const { return m_last - m_first; }
    const T &operator[](std::size_t i) const { return m_first[i]; }
};

}
#endif
