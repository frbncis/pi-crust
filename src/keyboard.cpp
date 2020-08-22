#include <keyboard.h>
#include <X11/keysymdef.h>
#include "scancodes.h"
#include "hid.h"
#include <iostream>

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

    std::cout << "Sending reports\n";

    for (int i = 0; i < this->pressed_keys.size(); i++)
    {
        struct x11_keysym *s = toscan2(this->pressed_keys[i]);

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

        send_key(hid_pipe, l->key, l->mod);
        send_key(hid_pipe, '\0', '\0'); //release all keys
        
        if (l->is_dead)
        {
            //dead keys need to be pressed twice to show up
            send_key(hid_pipe, l->key, l->mod);
            send_key(hid_pipe, '\0', '\0'); //release all keys
        }
    }

    this->pressed_keys.clear();

    fclose(hid_pipe);
}