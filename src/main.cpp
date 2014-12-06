/* Copyright 2014 Dietrich Epp.
   This file is part of Legend of Feleria.  Legend of Feleria is
   licensed under the terms of the 2-clause BSD license.  For more
   information, see LICENSE.txt. */
#include "sg/entry.h"
#include "sg/event.h"
#include "sg/keycode.h"
#include "sg/mixer.h"
#include "sg/record.h"
#include "game/game.hpp"
#include "graphics/system.hpp"

namespace {

Game::Game *game;
Graphics::System *graphics;

}

void sg_game_init(void) {
    game = new Game::Game;
}

void sg_game_destroy(void) {}

void sg_game_getinfo(struct sg_game_info *info) {
    info->name = "Legend of Feleria";
}

void sg_game_event(union sg_event *evt) {
    switch (evt->common.type) {
    case SG_EVENT_VIDEO_INIT:
        if (graphics) {
            delete graphics;
            graphics = nullptr;
        }
        graphics = new Graphics::System;
        graphics->load();
        break;

    case SG_EVENT_KDOWN:
        switch (evt->key.key) {
        case KEY_Backslash:
            sg_record_screenshot();
            return;

        case KEY_F10:
            sg_record_start(evt->common.time);
            return;

        case KEY_F11:
            sg_record_stop();
            return;
        }
        break;

    default:
        break;
    }

    game->handle_event(*evt);
}

void sg_game_draw(int width, int height, double time) {
    sg_mixer_settime(time);
    game->update(time);
    sg_mixer_commit();
    graphics->draw(width, height, *game);
}
