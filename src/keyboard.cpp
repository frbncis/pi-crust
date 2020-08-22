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
    this->pressed_keys[key_sym] = true;
}

void Keyboard::key_up_handler(unsigned long key_sym)
{
    this->held_keys[key_sym] = false;
    this->released_keys[key_sym] = true;
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

        // send_key(hid_pipe, l->key, l->mod);

        report_buffer[i] = l->key;
    }

    // this->pressed_keys.clear();
    char report_output[20];

    sprintf(
        report_output,
        "%x %x %x %x %x %x %x %x",
        this->get_modifier_status(held_keys), 
        '\0', 
        report_buffer[0], 
        report_buffer[1],
        report_buffer[2],
        report_buffer[3],
        report_buffer[4],
        report_buffer[5]);

    std::cout << report_output << "\n";

    fclose(hid_pipe);
}

bool Keyboard::is_modifier(unsigned long key_sym)
{
    return key_sym == XK_Control_L || key_sym == XK_Shift_L || key_sym == XK_Alt_L || key_sym == XK_Super_L || key_sym == XK_Control_R || key_sym == XK_Shift_R || key_sym == XK_Alt_R || key_sym == XK_Super_R;
}

unsigned short Keyboard::get_modifier_status(std::map<KeySym, bool> held_keys)
{
    return (held_keys[XK_Control_L] && (1 << 0));
}