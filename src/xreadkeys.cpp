#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scancodes.h"
#include <X11/XKBlib.h>
#include <X11/extensions/XInput2.h>

#include "keyboard.h"
#include "mouse.h"

enum params
{                   //argv-indices:
    P_EXE,          //executable name
    P_DEV_KEYBOARD, // keyboard device file
    P_DEV_MOUSE,    // mouse device file
    NUM_P           //number of parameters
};

enum errors
{
    ERR_SUCCESS,  //no error
    ERR_ARGCOUNT, //wrong number of arguments
    ERR_SYMBOL,   //symbol not in look up table
    ERR_LAYOUT,   //parameter P_LAY does not contain a correct keyboard layout
    ERR_LAZY      //i haven't done this
};

int main(int argc, char **argv)
{
    if (argc != NUM_P)
    {
        fprintf(stderr, "Usage: %s <device file> <layout> <unicode>\n", argv[P_EXE]);
        fprintf(stderr, "Takes string to type from stdin\n");
        fprintf(stderr, "<device file>:\ton the Raspberry Pi usually /dev/hidg0\n");

        return ERR_ARGCOUNT;
    }

    Display *display;
    Window window;
    XEvent event;
    XIEvent *xi_event;
    XIRawEvent *xev;
    XGenericEventCookie *cookie = &event.xcookie;

    Keyboard kb = Keyboard(argv[P_DEV_KEYBOARD]);
    Mouse mouse = Mouse(argv[P_DEV_MOUSE]);

    printf("Starting...\n");

    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    /* create fullscreen window */
    window = XRootWindow(display, 0);

    /*
     * Set mask to receive XI_RawMotion events. Because it's raw,
     * XWarpPointer() events are not included, you can use XI_Motion
     * instead.
     */
    unsigned char mask_bytes[(XI_LASTEVENT + 7) / 8] = {0}; /* must be zeroed! */
    XISetMask(mask_bytes, XI_RawMotion);
    XISetMask(mask_bytes, XI_RawButtonPress);
    XISetMask(mask_bytes, XI_RawButtonRelease);
    XISetMask(mask_bytes, XI_RawKeyPress);
    XISetMask(mask_bytes, XI_RawKeyRelease);

    /* select kind of events we are interested in */
    /* Set mask to receive events from all master devices */
    XIEventMask evmasks[1];
    /* You can use XIAllDevices for XWarpPointer() */
    evmasks[0].deviceid = XIAllMasterDevices;
    evmasks[0].mask_len = sizeof(mask_bytes);
    evmasks[0].mask = mask_bytes;

    XISelectEvents(display, window, evmasks, 1);

    printf("Starting event loop...\n");

    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);

        if (event.xcookie.type != GenericEvent)
        {
            /* not an XInput event */
            continue;
        }

        XGetEventData(display, &event.xcookie);

        if (event.xcookie.evtype == XI_RawMotion)
        {
            XFreeEventData(display, &event.xcookie);

            Window root_return, child_return;
            int root_x_return, root_y_return;
            int win_x_return, win_y_return;
            unsigned int mask_return;

            /*
            * We need:
            *     child_return - the active window under the cursor
            *     win_{x,y}_return - pointer coordinate with respect to root window
            */
            int retval = XQueryPointer(display, window, &root_return, &child_return,
                                       &root_x_return, &root_y_return,
                                       &win_x_return, &win_y_return,
                                       &mask_return);
            if (!retval)
            {
                /* pointer is not in the same screen, ignore */
                continue;
            }

            mouse.update_position(root_x_return, root_y_return);
            mouse.send_mouse_report();

            printf("XI_RawMotion: x %d y %d\n", root_x_return, root_y_return);
        }
        else if (event.xcookie.evtype == XI_RawButtonPress)
        {
            xi_event = (XIEvent *)cookie->data;
            xev = (XIRawEvent *)xi_event;

            printf("XI_RawButtonPress: %d \n", xev->detail);

            mouse.button_pressed_handler(xev->detail);
            mouse.send_mouse_report();
        }
        else if (event.xcookie.evtype == XI_RawButtonRelease)
        {
            xi_event = (XIEvent *)cookie->data;
            xev = (XIRawEvent *)xi_event;

            printf("XI_RawButtonRelease: %d \n", xev->detail);

            mouse.button_released_handler(xev->detail);
            mouse.send_mouse_report();
        }
        else if (event.xcookie.evtype == XI_RawKeyPress)
        {
            xi_event = (XIEvent *)cookie->data;
            xev = (XIRawEvent *)xi_event;

            // xev->detail returns the keycode
            auto keysym = XKeycodeToKeysym(display, xev->detail, 0);

            printf("XI_RawKeyPress: %d \n", keysym);
            kb.key_down_handler(keysym);
        }
        else if (event.xcookie.evtype == XI_RawKeyRelease)
        {
            xi_event = (XIEvent *)cookie->data;
            xev = (XIRawEvent *)xi_event;

            // xev->detail returns the keycode
            auto keysym = XKeycodeToKeysym(display, xev->detail, 0);

            printf("XI_RawKeyRelease: %d \n", xev->detail);
            kb.key_up_handler(xev->detail);
        }

        kb.send_keyboard_reports();
    }

    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}
