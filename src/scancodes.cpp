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
        {XK_e, 0x08},
        {XK_f, 0x09},
        {XK_g, 0x0A},
        {XK_h, 0x0B},
        {XK_i, 0x0C},
        {XK_j, 0x0D},
        {XK_k, 0x0E},
        {XK_l, 0x0F},
        {XK_m, 0x10},
        {XK_n, 0x11},
        {XK_o, 0x12},
        {XK_p, 0x13},
        {XK_q, 0x14},
        {XK_r, 0x15},
        {XK_s, 0x16},
        {XK_t, 0x17},
        {XK_u, 0x18},
        {XK_v, 0x19},
        {XK_w, 0x1A},
        {XK_x, 0x1B},
        {XK_y, 0x1C},
        {XK_z, 0x1D},
	{XK_space, 0x2C},
        {XK_BackSpace, 0x2A},
        {XK_Return, 0x28},
        {XK_Super_L, 0x08}};

unsigned char *toscan2(unsigned int x11_keycode)
{
//    printf("Looking for 0x%x\n", x11_keycode);

    auto scan_code = x11_keysyms[x11_keycode];

    if (scan_code == '\0')
    {
        return NULL;
    }
    else
    {
        return &x11_keysyms[x11_keycode];
    }
}
