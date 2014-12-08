/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_SCRIPT_HPP
#define LD_GAME_SCRIPT_HPP
#include "base/file.hpp"
#include "base/range.hpp"
namespace Game {

class Script {
private:
    Base::Data m_data;

    Base::Range<char[16]> m_labelname;
    Base::Range<unsigned short> m_labelpos;
    Base::Range<char> m_text;
    Base::Range<unsigned short> m_prog;
    Base::Range<char[16]> m_varname;

public:
    // ============================================================
    // Entry points
    // ============================================================

    Script();

    /// Load the sprite data.
    bool load();

    // ============================================================
    // Queries
    // ============================================================

    int get_label(const std::string &name) const;
    const char *get_text(int index) const;
    Base::Range<unsigned short> program() const {
        return m_prog;
    }
    int memory_size() const {
        return m_varname.size();
    }
};

}
#endif
