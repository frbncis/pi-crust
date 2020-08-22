#include <keyboard.h>
#include <X11/keysymdef.h>
#include "scancodes.h"
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

#define MAX_KEYPRESSES_REPORTED 6

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
    this->held_keys.erase(key_sym);
}

void Keyboard::send_keyboard_reports()
{
    FILE *hid_pipe = fopen(this->hid_device.c_str(), "w");

//    std::cout << "Sending report\n";

    // Note that X11 only reports a maximum of three non-modifier keypresses.
    // The maximum defined is is the USB HID maximum.
    unsigned char report_buffer[MAX_KEYPRESSES_REPORTED] = {'\0', '\0', '\0', '\0', '\0', '\0'};

    int buffer_position = 0;

    for (auto const &[held_key, is_held] : this->held_keys)
    {
        if (is_held == true && !is_modifier(held_key))
        {
            unsigned char *s = get_scancode(held_key);

            if (s != NULL)
            {
                report_buffer[buffer_position++] = *s;
            }
        }

        if (buffer_position > MAX_KEYPRESSES_REPORTED)
        {
            std::cout << "Ignoring extra pressed keys\n";
            break;
        }
    }

//    char report[16];
//
//    sprintf(
//        report,
//        "%x %x %x %x %x %x %x %x",
//        this->get_modifier_report(held_keys),
//        '\0',
//        report_buffer[0],
//        report_buffer[1],
//        report_buffer[2],
//        report_buffer[3],
//        report_buffer[4],
//        report_buffer[5]);
//
//    std::cout << report << "\n";

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
    return key_sym == XK_Control_L ||
           key_sym == XK_Shift_L ||
           key_sym == XK_Alt_L ||
           key_sym == XK_Super_L ||
           key_sym == XK_Control_R ||
           key_sym == XK_Shift_R ||
           key_sym == XK_Alt_R ||
           key_sym == XK_Super_R;
}

unsigned char Keyboard::get_modifier_report(std::map<KeySym, bool> held_keys)
{
    return (held_keys[XK_Control_L] << 0) +
           (held_keys[XK_Shift_L] << 1) +
           (held_keys[XK_Alt_L] << 2) +
           (held_keys[XK_Super_L] << 3) +
           (held_keys[XK_Control_R] << 4) +
           (held_keys[XK_Shift_R] << 5) +
           (held_keys[XK_Alt_R] << 6) +
           (held_keys[XK_Super_R] << 7);
}
