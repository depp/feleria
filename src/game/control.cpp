/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "control.hpp"
#include "sg/entry.h"
#include "sg/event.h"
#include "sg/keycode.h"
#include <algorithm>
namespace Game {
namespace Control {

namespace {

const struct ButtonBinding STANDARD_KEY[] = {
    { KEY_W,      Button::MOVE_UP },
    { KEY_A,      Button::MOVE_LEFT },
    { KEY_S,      Button::MOVE_DOWN },
    { KEY_D,      Button::MOVE_RIGHT },

    { KEY_Up,     Button::MOVE_UP },
    { KEY_Left,   Button::MOVE_LEFT },
    { KEY_Down,   Button::MOVE_DOWN },
    { KEY_Right,  Button::MOVE_RIGHT },

    { KEY_Space,  Button::ACTION_1 },
    { KEY_Tab,    Button::ACTION_2 },
    { KEY_X,      Button::ACTION_1 },
    { KEY_Z,      Button::ACTION_2 },
    { KEY_Period, Button::ACTION_1 },
    { KEY_Comma,  Button::ACTION_2 },

    { KEY_Escape, Button::MENU }
};

}

/* ======================================================================
   Input::State
   ====================================================================== */

/// Snapshot of player input at a moment in time.
struct Input::State {
    /// State timestamp.
    double time;
    /// The state of all buttons.
    unsigned buttons;
    /// Horizontal movement vector, a velocity.
    Vec2 move;

    /// Get the state of a button.
    bool get_button(Button button) const;
    /// Get the value of an axis controlled by buttons.
    float get_axis(Button negative, Button positive) const;
    /// Get the movement vector.
    Vec2 get_move() const;
    /// Create a copy of this state with a different time.
    State clone(double new_time) const;
};

bool Input::State::get_button(Button button) const {
    return (buttons & button_mask(button)) != 0;
}

float Input::State::get_axis(Button negative, Button positive) const {
    return (get_button(negative) ? -1.0f : 0.0f) +
        (get_button(positive) ? +1.0f : 0.0f);
}

Vec2 Input::State::get_move() const {
    Vec2 keymove { get_axis(Button::MOVE_LEFT, Button::MOVE_RIGHT),
                   get_axis(Button::MOVE_UP, Button::MOVE_DOWN) };
    float mag2 = keymove.mag2();
    if (mag2 > 1.0f)
        keymove *= 1.0f / std::sqrt(mag2);
    return keymove;
}

Input::State Input::State::clone(double new_time) const {
    return State { new_time, buttons, move };
}

/* ======================================================================
   Input::ButtonPress
   ====================================================================== */

/// Record of a button being pressed.
struct Input::ButtonPress {
    /// Device where the button was pressed, e.g., to distinguish
    /// keyboard from gamepad.
    int source;
    /// Identity of the button that was pressed, e.g., to distinguish
    /// different keys on a keyboard.
    int ident;
    /// The button which was pressed.
    Button target;
};

/* ======================================================================
   Input
   ====================================================================== */

Input::Input() : m_readpos(0) {
    m_state.push_back(State { 0.0, 0, Vec2::zero() });
}

Input::~Input() { }

void Input::button_press(double time, int source, int ident, Button button) {
    for (auto &b : m_button) {
        if (source == b.source && ident == b.ident)
            return;
    }

    m_button.push_back(ButtonPress { source, ident, button });
    buttons_set(time, m_state.back().buttons | button_mask(button));
}

void Input::button_release(double time, int source, int ident) {
    Button button;

    {
        auto i = std::begin(m_button), e = std::end(m_button);
        for (; i != e; i++) {
            if (source == i->source && ident == i->ident)
                break;
        }
        if (i == e)
            return;
        button = i->target;
        m_button.erase(i);
    }

    {
        auto i = std::begin(m_button), e = std::end(m_button);
        for (; i != e; i++) {
            if (i->target == button)
                return;
        }
    }

    buttons_set(time, m_state.back().buttons & ~button_mask(button));
}

FrameInput Input::read(double start_time, double end_time, bool do_remove) {
    auto sts = std::begin(m_state) + m_readpos,
        ste = std::end(m_state), stp = sts;
    unsigned new_buttons = 0, all_buttons = sts->buttons,
        cur_buttons = sts->buttons;
    double cur_time = start_time;
    Vec2 move = Vec2::zero();
    while (true) {
        Vec2 smove = stp->get_move();
        stp++;
        if (stp == ste) {
            float sdelta = (float) (end_time - cur_time);
            move += smove * sdelta;
            break;
        } else {
            float sdelta = (float) (stp->time - cur_time);
            move += smove * sdelta;
            new_buttons |= stp->buttons & ~cur_buttons;
            all_buttons |= stp->buttons;
            cur_buttons = stp->buttons;
            cur_time = stp->time;
        }
    }
    float scale = 1.0f / (float) (end_time - start_time);
    if (do_remove) {
        m_readpos = stp - sts - 1;
    }
    return FrameInput {
        all_buttons,
        new_buttons,
        move * scale
    };
}

void Input::buttons_set(double time, unsigned buttons) {
    State &cur = m_state.back();
    if (cur.buttons == buttons)
        return;
    if (cur.time >= time) {
        if (m_state.size() >= 2) {
            State &prev = m_state[m_state.size() - 2];
            unsigned diff = buttons ^ cur.buttons;
            if ((prev.buttons & diff) == (cur.buttons & diff)) {
                cur.buttons = buttons;
                return;
            }
        }
        time = cur.time;
    }
    State st = cur.clone(time);
    st.buttons = buttons;
    push_state(st);
}

void Input::push_state(const State &st) {
    if (m_readpos > 0) {
        m_state.erase(std::begin(m_state), std::begin(m_state) + m_readpos);
        m_readpos = 0;
    }
    m_state.push_back(st);
}

/* ======================================================================
   ButtonBindings
   ====================================================================== */

ButtonBindings::ButtonBindings() { }

ButtonBindings::~ButtonBindings() { }

void ButtonBindings::clear() {
    m_map.clear();
}

void ButtonBindings::bind(int source, Button target) {
    int target_idx = static_cast<int>(target);
    if (source < 0 || source >= 256 ||
        target_idx < 0 || target_idx >= BUTTON_COUNT)
        sg_sys_abort("Control::ButtonBindings::bind");
    std::size_t s = source;
    if (s >= m_map.size())
        m_map.resize(s + 1, -1);
    m_map[s] = target_idx;
}

int ButtonBindings::get(int source) const {
    if (source < 0 || static_cast<std::size_t>(source) >= m_map.size())
        return -1;
    return m_map[source];
}

/* ======================================================================
   Bindings
   ====================================================================== */

Bindings::Bindings() {
    m_button_map[0].bind(
        std::begin(STANDARD_KEY),
        std::end(STANDARD_KEY));
}

Bindings::~Bindings() { }

bool Bindings::handle_event(Input &input, const sg_event &evt) {
    switch (evt.common.type) {
    case SG_EVENT_KDOWN:
    case SG_EVENT_KUP:
        return handle_button(input, evt.key.time, 0, evt.key.key,
                             evt.common.type == SG_EVENT_KDOWN);

    default:
        return false;
    }
}

bool Bindings::handle_button(Input &input, double time, int device, int ident,
                             bool state) {
    if (state) {
        int button = m_button_map[device].get(ident);
        if (button >= 0) {
            input.button_press(time, device, ident,
                               static_cast<Button>(button));
        }
    } else {
        input.button_release(time, device, ident);
    }
    return true;
}

}
}
