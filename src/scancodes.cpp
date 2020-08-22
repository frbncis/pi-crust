// (C) 2016 Tobias Girstmair, released under the GNU GPL
#include <X11/Xlib.h>
#include <string.h>
#include "scancodes.h"
#include <X11/keysymdef.h>
#include <stdio.h>
#include <unordered_map>

std::unordered_map<KeySym, unsigned char> x11_keysyms =
    {
        {XK_a, 0x04},
        {XK_b, 0x05},
        {XK_c, 0x06},
        {XK_d, 0x07},
        {XK_s, 0x16},
        {XK_z, 0x1d},
        {XK_BackSpace, 0x2a},
        {XK_Return, 0x28},
        {XK_Super_L, 0x08}};

unsigned char *toscan2(unsigned int x11_keycode)
{
    printf("Looking for 0x%x\n", x11_keycode);

    return &x11_keysyms[x11_keycode];
}