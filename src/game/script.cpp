/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "script.hpp"
#include "base/chunk.hpp"
#include <cstring>
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
      m_memory() { }

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
    if (!s.m_labelname.size() ||
        s.m_labelname.size() != s.m_labelpos.size()) {
        return false;
    }
    s.m_text = chunks.get_array<char>("TEXT");
    s.m_prog = chunks.get_array<unsigned short>("PROG");
    s.m_memory.resize(chunks.get_array<char[16]>("VNAM").size());

    *this = std::move(s);
    return true;
}

}
