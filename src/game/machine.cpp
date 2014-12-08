/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "machine.hpp"
#include "script.hpp"
#include "game.hpp"
#include "person.hpp"
namespace Game {

namespace {
const int MACHINE_SPEED = 1000;

#include "data/opcode.enum.hpp"
#include "data/opcode.array.hpp"

int opcode_val(Opcode op) {
    return 0x8000 | static_cast<int>(op);
}

class ProgReader {
private:
    Base::Range<unsigned short> m_prog;
    const unsigned short *m_pc;
    int m_opcode;

    bool check_pc() const {
        if (m_pc == nullptr) {
            return false;
        }
        if (m_pc == m_prog.end()) {
            Log::error("Invalid PC");
            return false;
        }
        return true;
    }

    bool check_pc(int pc) const {
        if (pc < 0 || (std::size_t) pc >= m_prog.size()) {
            Log::error("Invalid PC");
            return false;
        }
        return true;
    }

public:
    ProgReader(const Script &script, int pc)
        : m_prog(script.program()), m_pc(nullptr), m_opcode(-1) {
        if (pc != -1) {
            if (check_pc(pc)) {
                m_pc = m_prog.begin() + pc;
            }
        }
    }

    bool is_halted() const {
        return m_pc == nullptr;
    }

    Opcode opcode() {
        if (!check_pc()) {
            return Opcode::EXIT;
        }
        int data = *m_pc++;
        if ((data & 0x8000) == 0) {
            error("expected opcode");
            return Opcode::EXIT;
        }
        data &= 0x7fff;
        if (data >= OPCODE_COUNT) {
            error("invalid opcode");
            return Opcode::EXIT;
        }
        m_opcode = data;
        return static_cast<Opcode>(data);
    }

    int imm() {
        if (!check_pc()) {
            return -1;
        }
        int data = *m_pc++;
        if ((data & 0x8000) != 0) {
            error("expected operand");
            return -1;
        }
        return data;
    }

    void jump(int addr) {
        if (!check_pc(addr)) {
            m_pc = nullptr;
        } else {
            m_pc = m_prog.begin() + addr;
        }
    }

    void jump(const unsigned short *ptr) {
        if (ptr == m_prog.end()) {
            Log::error("Invalid jump");
            m_pc = nullptr;
        } else {
            m_pc = ptr;
        }
    }

    void halt() {
        m_pc = nullptr;
    }

    void error(const char *msg) {
        const char *opname = m_opcode >= 0 ?
            OPCODE_NAMES[m_opcode] : "<none>";
        Log::error("$%04x: %s (after %s)",
                   (int) (m_pc - 1 - m_prog.begin()), msg, opname);
        m_pc = nullptr;
    }

    int get_pc() const {
        if (m_pc == nullptr) {
            return -1;
        }
        return (int)(m_pc - m_prog.begin());
    }

    const unsigned short *begin() const {
        return m_pc;
    }

    const unsigned short *end() const {
        if (m_pc == nullptr) {
            return nullptr;
        }
        return m_prog.end();
    }

    int addr(const unsigned short *ptr) {
        return (int) (ptr - m_prog.begin());
    }
};

}

Machine::Machine(const Script &script)
    : m_script(script) {
    reset();
}

void Machine::reset() {
    m_pc = -1;
    m_character = -1;
}

bool Machine::jump(const std::string &name) {
    int target = m_script.get_label(name);
    if (target < 0) {
        return false;
    }
    m_pc = target;
    return true;
}

#define ERROR(x) do { \
        Log::error("$%04x: %s", m_pc - 1, x); \
        m_pc = -1; \
        return; \
    } while (0)

void Machine::run(Game &game) {
    (void) &game;
    {
        int sz = m_script.memory_size();
        m_memory.resize(sz, 0);
    }
    int icount = 0;
    ProgReader r(m_script, m_pc);
    while (!r.is_halted()) {
        if (icount++ >= MACHINE_SPEED) {
            Log::warn("Instruction limit hit... infinite loop?");
            break;
        }

        switch (r.opcode()) {
        case Opcode::END:
            Log::warn("unexpected END opcode");
            break;

        case Opcode::EXIT:
            r.halt();
            goto exit;

        case Opcode::FADE: {
            int data = r.imm();
            float dtime = (1.0f / 30.0f) * (float) data;
            Log::debug("Fade: %f s", dtime);
            break;
        }

        case Opcode::GOTO: {
            int target = r.imm();
            if (target >= 0) {
                r.jump(target);
            }
            break;
        }

        case Opcode::INPUT:
            r.error("INPUT");
            break;

        case Opcode::RESET:
            r.error("RESET");
            break;

        case Opcode::RESPONSE: {
            auto p = r.begin(), e = r.end();
            int val = opcode_val(Opcode::END);
            for (; p != e; p++) {
                if (*p == val) {
                    r.jump(p + 1);
                    break;
                }
            }
            if (p == e) {
                r.error("unexpected response");
            }
            break;
        }

        case Opcode::SAVE:
            r.error("SAVE");
            break;

        case Opcode::SAY:
            r.error("SAY");
            break;

        case Opcode::SETPLAYER: {
            int name = r.imm();
            for (auto &p : game.person()) {
                if (p.identity() != name) {
                    p.set_player(false);
                } else {
                    p.set_player(true);
                    name = -1;
                }
            }
            break;
        }

        case Opcode::SETVAR: {
            int var = r.imm();
            int value = r.imm();
            set_var(var, value);
            break;
        }

        case Opcode::SPAWN: {
            int name = r.imm();
            int x = r.imm();
            int y = r.imm();
            Vec2 pos = Vec2 {{ (float) x, (float) y }};
            pos -= game.world().center();
            Person p(name, pos, Direction::DOWN);
            game.add_person(p);
            break;
        }

        case Opcode::SPRITE: {
            int name = r.imm();
            int part = r.imm();
            int sidx = r.imm();
            if (part < 0 || part >= PART_COUNT) {
                Log::error("Invalid sprite part");
                break;
            }
            if (sidx == 0x7fff) {
                sidx = -1;
            } else {
                const char *sname = m_script.get_text(sidx);
                if (sname == nullptr) {
                    sidx = -1;
                } else {
                    sidx = game.sprites().get_index(sname);
                }
            }
            for (auto &p : game.person()) {
                if (p.identity() != name) {
                    continue;
                }
                p.set_part(static_cast<Part>(part), sidx);
            }
            break;
        }
        }
    }
exit:
    m_pc = r.get_pc();
}

void Machine::set_var(int var, int value) {
    if (var < 0 || (std::size_t) var >= m_memory.size()) {
        Log::error("Invalid variable: %d", var);
        return;
    }
    m_memory[var] = value;
}

}
