#include <keyboard.h>
#include <X11/keysymdef.h>
#include "scancodes.h"
#include "hid.h"
#include <iostream>

#define MOD_NONE 0
#define MOD_LCTRL 1 << 0
#define MOD_LSHIFT 1 << 1
#define MOD_LALT 1 << 2
#define MOD_LSUPER 1 << 3
#define MOD_RCTRL 1 << 4
#define MOD_RSHIFT 1 << 5
#define MOD_RALT 1 << 6
#define MOD_RSUPER 1 << 7

Keyboard::Keyboard(std::string hid_device)
{
    this->hid_device = hid_device;
}

void Keyboard::key_down_handler(unsigned long key_sym)
{
    this->held_keys[key_sym] = true;
}

void Keyboard::key_up_handler(unsigned long key_sym)
{
    this->held_keys[key_sym] = false;
}

void Keyboard::send_keyboard_reports()
{
    FILE *hid_pipe = fopen(this->hid_device.c_str(), "w");

    std::cout << "Sending report\n";

    unsigned long held_key_buffer[6] = { 0, 0, 0, 0, 0, 0 };
    unsigned long report_buffer[8] = { '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'};

    int buffer_position = 0;

    for (auto const& [held_key, is_held]: this->held_keys)
    {
        if (is_held == true && !is_modifier(held_key))
        {
            held_key_buffer[buffer_position++] = held_key;
        }

        if (buffer_position > 6)
        {
            break;
        }
    }

    for (int i = 0; i < 6; i++)
    {
        if (held_key_buffer[i] == NULL)
        {
            continue;
        }

        struct x11_keysym *s = toscan2(held_key_buffer[i]);

        if (s == NULL)
        {
            printf("Key symbol not found.\n");
            continue;
        }

        struct layout *l = tolay(s, en_US);
        if (l == NULL)
        {
            fprintf(stderr, "Unrecognised keyboard layout.\n");
            continue;
        }

        report_buffer[i] = l->key;
    }

    fprintf(
        hid_pipe,
        "%c%c%c%c%c%c%c%c",
        this->get_modifier_report(held_keys), 
        '\0', 
        report_buffer[0], 
        report_buffer[1],
        report_buffer[2],
        report_buffer[3],
        report_buffer[4],
        report_buffer[5]);

    fclose(hid_pipe);
}

bool Keyboard::is_modifier(unsigned long key_sym)
{
    return key_sym == XK_Control_L || key_sym == XK_Shift_L || key_sym == XK_Alt_L || key_sym == XK_Super_L || key_sym == XK_Control_R || key_sym == XK_Shift_R || key_sym == XK_Alt_R || key_sym == XK_Super_R;
}

unsigned long Keyboard::get_modifier_report(std::map<KeySym, bool> held_keys)
{
    return
        (held_keys[XK_Control_L] << 0) +
        (held_keys[XK_Shift_L]   << 1) +
        (held_keys[XK_Alt_L]     << 2) +
        (held_keys[XK_Super_L]   << 3) +
        (held_keys[XK_Control_R] << 4) +
        (held_keys[XK_Shift_R]   << 5) +
        (held_keys[XK_Alt_R]     << 6) +
        (held_keys[XK_Super_R]   << 7);
}