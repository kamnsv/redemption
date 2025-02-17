/*
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Product name: redemption, a FLOSS RDP proxy
   Copyright (C) Wallix 2010-2013
   Author(s): Clément Moroldo, David Fort
*/

#pragma once

#include "keyboard/keymap2.hpp"
#include "client_redemption/client_redemption_api.hpp"
#include "mod/internal/replay_mod.hpp"


class ClientCallback
{
private:
    Keymap2           keymap;

    mod_api            * mod = nullptr;
    ClientRedemptionAPI * client;

    ReplayMod * replay_mod = nullptr;

public:
    struct MouseData {
        uint16_t x = 0;
        uint16_t y = 0;
    } mouse_data;

    ClientCallback(ClientRedemptionAPI * client)
    : client(client)
    {}

    mod_api * get_mod() {
        return this->mod;
    }

    void set_mod(mod_api * mod) {
        this->mod = mod;
    }

    void init() {
        this->mod = nullptr;
    }

    // REPLAY
    void delete_replay_mod() {
        this->client->delete_replay_mod();
    }

    void replay(const std::string & movie_path) {
        this->client->replay(movie_path);
    }

    bool load_replay_mod(MonotonicTimePoint begin, MonotonicTimePoint end) {
        return this->client->load_replay_mod(begin, end);
    }

    void set_replay(ReplayMod * replay_mod) {
        this->replay_mod = replay_mod;
    }

    // Controller
    void init_layout(int lcid) {
        this->keymap.init_layout(lcid);
    }

    void connect(const std::string& ip, const std::string& name, const std::string& pwd, const int port) {
        this->client->connect(ip, name, pwd, port);
    }

    void close() {
        if (this->get_mod() != nullptr) {
            this->mod->disconnect();
            this->mod = nullptr;
        }
        this->client->close();
    }

    void disconnect(bool pipe_broken) {
        if (this->get_mod() != nullptr) {
            if (!pipe_broken) {
                this->mod->disconnect();
            }
            this->mod = nullptr;
        }
        this->client->disconnect("", false);
    }

    void refreshPressed(const uint16_t width, const uint16_t height) {
        if (this->mod != nullptr) {
            Rect rect(0, 0, width, height);
            this->mod->rdp_input_invalidate(rect);
        }
    }

    void CtrlAltDelPressed() {
        const int flag = Keymap2::KBDFLAGS_EXTENDED;

        this->send_rdp_scanCode(KBD_SCANCODE_ALTGR , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_CTRL  , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_DELETE, flag);
    }

    void CtrlAltDelReleased() {
        const int flag = int(Keymap2::KBDFLAGS_EXTENDED) | int(KBD_FLAG_UP);

        this->send_rdp_scanCode(KBD_SCANCODE_ALTGR , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_CTRL  , flag);
        this->send_rdp_scanCode(KBD_SCANCODE_DELETE, flag);
    }

    void mouseButtonEvent(int x, int y, int flag) {
        if (this->mod != nullptr && flag != MOUSE_FLAG_BUTTON4) {

            this->mod->rdp_input_mouse(flag, x, y, &(this->keymap));
        }
    }

    void wheelEvent(int delta) {
        int flag(0);
        if (delta < 0) {
            flag = MOUSE_FLAG_WHEEL | MOUSE_FLAG_WHEEL_NEGATIVE;
        } else {
            flag = MOUSE_FLAG_BUTTON4;
        }

        if (this->mod != nullptr) {
            this->mod->rdp_input_mouse(flag, 0, 0, &(this->keymap));
        }
    }

    bool mouseMouveEvent(int x, int y) {

        if (this->mod != nullptr /*&& y < this->config.info.height*/) {
            this->mouse_data.x = x;
            this->mouse_data.y = y;
            this->mod->rdp_input_mouse(MOUSE_FLAG_MOVE, this->mouse_data.x, this->mouse_data.y, &(this->keymap));
        }

        return false;
    }

    void send_rdp_scanCode(uint16_t keyCode, uint16_t flag) {
        if (this->mod != nullptr) {
            this->mod->rdp_input_scancode(keyCode, 0, flag, /*timer=*/0, &(this->keymap));
        }
    }

    void send_rdp_unicode(uint16_t unicode, uint16_t flag) {
        this->mod->rdp_input_unicode(unicode, flag);
    }
};
