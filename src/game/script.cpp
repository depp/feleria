/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "script.hpp"
#include "base/chunk.hpp"
#include <cstring>
#include "defs.hpp"
namespace Game {

namespace {

const char SCRIPT_MAGIC[16] = "Feleria Script";

}

Script::Script()
    : m_data(),
      m_labelname(nullptr, nullptr),
      m_labelpos(nullptr, nullptr),
      m_text(nullptr, nullptr),
      m_prog(nullptr, nullptr),
      m_varname(nullptr, nullptr) { }

bool Script::load() {
    Script s;
    s.m_data.read("script.dat", 1u << 20);
    Base::ChunkReader chunks;
    if (!chunks.read(s.m_data) ||
        std::memcmp(chunks.magic(), SCRIPT_MAGIC, sizeof(SCRIPT_MAGIC)) ||
        chunks.version().first != 1) {
        return false;
    }

    s.m_labelname = chunks.get_array<char[16]>("LNAM");
    s.m_labelpos = chunks.get_array<unsigned short>("LPOS");
    s.m_text = chunks.get_array<char>("TEXT");
    s.m_prog = chunks.get_array<unsigned short>("PROG");
    s.m_varname = chunks.get_array<char[16]>("VNAM");
    if (!s.m_labelname.size() ||
        s.m_labelname.size() != s.m_labelpos.size() ||
        s.m_text.size() == 0 || *(s.m_text.end() - 1) != 0) {
        return false;
    }

    *this = std::move(s);
    return true;
}

int Script::get_label(const std::string &name) const {
    std::size_t i = 0, n = m_labelname.size();
    const char *cname = name.c_str();
    for (; i < n; i++) {
        if (!std::strcmp(cname, m_labelname[i])) {
            return m_labelpos[i];
        }
    }
    return -1;
}

const char *Script::get_text(int index) const {
    if (index < 0 || (std::size_t) index >= m_text.size() - 1) {
        Log::error("Invalid text index: %d", index);
        return nullptr;
    }
    return &m_text[index];
}

}
