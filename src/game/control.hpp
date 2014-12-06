/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#ifndef LD_GAME_CONTROL_HPP
#define LD_GAME_CONTROL_HPP
#include <utility>
#include <vector>
#include "defs.hpp"
union sg_event;
namespace Game {
namespace Control {

/// On/off control buttons.
enum class Button {
    // Movement
    MOVE_LEFT, MOVE_RIGHT, MOVE_UP, MOVE_DOWN,
    // Config / attack / interact
    ACTION_1,
    // Cancel / secondary attack
    ACTION_2,
    // Bring up menu
    MENU
 };

/// Number of on/off control buttons.
static const int BUTTON_COUNT = 7;

/// Get the bitmask for a button.
inline unsigned button_mask(Button button) {
    return 1u << static_cast<int>(button);
}

/// Information about input during a frame.
struct FrameInput {
    /// Set of buttons which were down at some point during the frame.
    unsigned buttons;
    /// Set of buttons which were pressed during the frame.
    unsigned new_buttons;
    /// Player movement vector.
    Vec2 move;
};

/// State of input, including recent history.
class Input {
private:
    struct State;
    struct ButtonPress;
    std::vector<State> m_state;
    std::vector<ButtonPress> m_button;
    std::size_t m_readpos;

public:
    Input();
    Input(const Input &) = delete;
    ~Input();
    Input &operator=(const Input &) = delete;

    /// Record a button press.
    void button_press(double time, int source, int ident, Button button);
    /// Record a button release.
    void button_release(double time, int source, int ident);
    /// Read a frame of input.
    FrameInput read(double start_time, double end_time, bool do_remove);

private:
    /// Set the button mask.
    void buttons_set(double time, unsigned buttons);
    /// Add a new state.
    void push_state(const State &st);
};

/// A binding for a button.
struct ButtonBinding {
    int source;
    Button target;
};

/// A map from device buttons to virtual buttons.
class ButtonBindings {
private:
    std::vector<signed char> m_map;

public:
    ButtonBindings();
    ButtonBindings(const ButtonBindings &) = delete;
    ~ButtonBindings();
    ButtonBindings &operator=(const ButtonBindings &) = delete;

    /// Clear all bindings.
    void clear();
    /// Bind a device button to a target.
    void bind(int source, Button target);
    /// Bind many device buttons to targets.
    template<class I>
    void bind(I first, I last);
    /// Get the target for a device button, or -1 if unbound.
    int get(int source) const;
};

template<class I>
void ButtonBindings::bind(I first, I last) {
    for (I p = first; p != last; ++p)
        bind(p->source, p->target);
}

/// Control bindings.
class Bindings {
private:
    ButtonBindings m_button_map[1];

public:
    Bindings();
    Bindings(const Bindings &) = delete;
    ~Bindings();
    Bindings &operator=(const Bindings &) = delete;

    /// Handle an event.  Returns true if the event was handled.
    bool handle_event(Input &input, const sg_event &evt);

private:
    bool handle_button(Input &input, double time, int device, int ident,
                       bool state);
};

}
}
#endif
