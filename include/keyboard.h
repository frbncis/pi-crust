#include <map>
#include <X11/Xlib.h>

#pragma once

class Keyboard {
public:
    Keyboard(std::string hid_device);

    void key_down_handler(unsigned long key_sym);
    void key_up_handler(unsigned long key_sym);

    void send_keyboard_reports();

private:
    std::map<KeySym, bool> held_keys;

    bool is_modifier(unsigned long key_sym);

    unsigned long get_modifier_report(std::map<KeySym, bool> held_keys);

    std::string hid_device;
};