#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scancodes.h"
#include <X11/XKBlib.h>

#include "keyboard.h"
#include "hid.h"

enum params
{          //argv-indices:
    P_EXE, //executable name
    P_DEV, //device file
    P_LAY, //layout
    P_UNI, //unicode method
    NUM_P  //number of parameters
};

enum uni_m
{              //unicode methods:
    SKIP,      //ignore any keys not on the layout
    GTK_HOLD,  //hold ctrl and shift while entering hex values
    GTK_SPACE, //end hex sequence with spacebar
    WINDOWS    //use alt+numpad
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
        fprintf(stderr, "<layout>:\n\t%d\t%s\n\t%d\t%s\n\t%d\t%s\n",
                en_US, "en_US",
                de_AT, "de_AT (w/ dead keys)",
                de_ND, "de_AT-nodeadkeys");
        fprintf(stderr, "<unicode>:\n\t%d\t%s\n\t%d\t%s\n\t%d\t%s\n\t%d\t%s\n",
                SKIP, "skip over unicode characters",
                GTK_HOLD, "X11 Holddown: CTRL+SHIFT+[u, hex]",
                GTK_SPACE, "X11 Space: CTRL+SHIFT+u, hex, SPACE",
                WINDOWS, "Windows: Alt+[Numpad]");
        return ERR_ARGCOUNT;
    }

    Display *display;
    Window window;
    XEvent event;
    Keyboard kb = Keyboard(argv[P_DEV]);

    int eventLoopRunOnce = 0;
    int s;

    printf("Starting...\n");
    // printf("Opening %s\n", argv[P_DEV]);

    /* open connection with the server */
    display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        fprintf(stderr, "Cannot open display\n");
        exit(1);
    }

    s = DefaultScreen(display);

    /* get the display width and height */
    int width = DisplayWidth(display, s);
    int height = DisplayHeight(display, s);

    /* create fullscreen window */
    window = XCreateSimpleWindow(display, RootWindow(display, s), 0, 0, width, height, 1,
                                 BlackPixel(display, s), WhitePixel(display, s));

    /* select kind of events we are interested in */
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask);

    /* map (show) the window */
    XMapWindow(display, window);

    printf("Starting event loop...\n");

    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);

        // FILE *hid_dev = NULL;

        /* keyboard events */
        if (event.type == KeyPress)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);

            char *sym_name = XKeysymToString(keysym);

            printf("KeyPress: %s [0x%lx]\n", sym_name, keysym);

            kb.key_down_handler(keysym);


            // fclose(hid_dev);

            /* exit on ESC key press */
            if (event.xkey.keycode == 0x09)
                break;
        }
        else if (event.type == KeyRelease)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);

            char *sym_name = XKeysymToString(keysym);

            printf("KeyRelease: %s [0x%lx]\n", sym_name, keysym);

            kb.key_up_handler(keysym);
        }
        /* dummy x11 screen entry/exit events */
        else if (event.type == EnterNotify && !eventLoopRunOnce)
        {
            eventLoopRunOnce = 1;
        }
        else if (event.type == EnterNotify && eventLoopRunOnce)
        {
            printf("EnterWindow\n");
        }
        else if (event.type == LeaveNotify)
        {
            printf("LeaveWindow\n");
        }

        kb.send_keyboard_reports();
    }

    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}