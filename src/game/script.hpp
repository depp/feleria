/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_SCRIPT_HPP
#define LD_GAME_SCRIPT_HPP

#include "base/file.hpp"
#include "base/range.hpp"
#include <vector>
namespace Game {

class Script {
private:
    Base::Data m_data;

    Base::Range<char[16]> m_labelname;
    Base::Range<unsigned short> m_labelpos;
    Base::Range<char> m_text;
    Base::Range<unsigned short> m_prog;

    std::vector<unsigned char> m_memory;

public:
    // ============================================================
    // Entry points
    // ============================================================

    Script();

    /// Load the sprite data.
    bool load();
};

}
#endif
