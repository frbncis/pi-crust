#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scancodes.h"
#include <X11/XKBlib.h>

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

void send_key(FILE *hid_dev, unsigned short key, unsigned short mod);
enum errors send_unicode(FILE *hid_dev, unsigned int unicode, enum uni_m method, enum kbdl layout);

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
    int eventLoopRunOnce = 0;
    int s;

    printf("Starting...\n");
    printf("Opening %s\n", argv[P_DEV]);

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

        FILE *hid_dev = NULL;
        if (event.type == KeyPress || event.type == KeyRelease)
        {
            hid_dev = fopen(argv[P_DEV], "w");
        }

        /* keyboard events */
        if (event.type == KeyPress)
        {
            KeySym keysym = XLookupKeysym(&event.xkey, 0);
            const char space[] = " ";
            const char *test = &space[0];

            char *sym_name = NULL;
            if (event.xkey.keycode == 0x41)
            {
                sym_name = test;
            }
            else
            {
                sym_name = XKeysymToString(keysym);
            }

            printf("KeyPress: %s [0x%x]\n", sym_name, keysym);

            struct keysym *s = toscan2(keysym);
            if (s == NULL)
            {
                printf("Key symbol not found.\n");
                continue;
            }

            struct layout *l = tolay(s, atoi(argv[P_LAY]));
            if (l == NULL)
            {
                fprintf(stderr, "Unrecognised keyboard layout.\n");
                continue;
            }

            //            if (l->key != 0x00) {
            send_key(hid_dev, l->key, l->mod);
            send_key(hid_dev, '\0', '\0'); //release all keys
            if (l->is_dead)
            {
                //dead keys need to be pressed twice to show up
                send_key(hid_dev, l->key, l->mod);
                send_key(hid_dev, '\0', '\0'); //release all keys
            }
            //            }

            /* exit on ESC key press */
            if (event.xkey.keycode == 0x09)
                break;
        }
        else if (event.type == KeyRelease)
        {
            // printf( "KeyRelease: %x\n", event.xkey.keycode );
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

        if (hid_dev != NULL)
        {
            fclose(hid_dev);
        }
    }

    /* close connection to server */
    XCloseDisplay(display);

    return 0;
}

void send_key(FILE *hid_dev, unsigned short key, unsigned short mod)
{
    if (hid_dev != NULL)
    {
        fprintf(hid_dev, "%c%c%c%c%c%c%c%c", mod, '\0', key, '\0', '\0', '\0', '\0', '\0');
    }
    else
    {
        printf(stderr, "Cannot write to device\n");
    }
}

enum errors send_unicode(FILE *hid_dev, unsigned int unicode, enum uni_m method, enum kbdl layout)
{
    char buf[10];
    struct keysym *s;
    struct layout *l;

    if (unicode == 0x00)
    {
        fprintf(stderr, "Symbol not in lookup table!\n");
        return ERR_SYMBOL;
    }

    switch (method)
    {
    case SKIP:
        break;
    case GTK_HOLD:
        sprintf(buf, "%x", unicode);
        s = toscan("u");
        l = tolay(s, layout);
        send_key(hid_dev, l->key, MOD_LCTRL | MOD_LSHIFT);
        for (int i = 0; i < strlen(buf); i++)
        {
            s = toscan((char[2]){buf[i], '\0'});
            l = tolay(s, layout);
            send_key(hid_dev, l->key, MOD_LCTRL | MOD_LSHIFT);
        }
        send_key(hid_dev, '\0', '\0');
        break;
    case GTK_SPACE:
        sprintf(buf, "%x ", unicode);
        s = toscan("u");
        l = tolay(s, layout);
        send_key(hid_dev, l->key, MOD_LCTRL | MOD_LSHIFT);
        for (int i = 0; i < strlen(buf); i++)
        {
            s = toscan((char[2]){buf[i], '\0'});
            l = tolay(s, layout);
            send_key(hid_dev, l->key, MOD_NONE);
        }
        send_key(hid_dev, '\0', '\0');
        break;
    case WINDOWS:
        fprintf(stderr, "windows method not implemented!\n");
        return ERR_LAZY;
    default:
        fprintf(stderr, "unknown unicode method!\n");
        return ERR_LAYOUT; //TODO: better error code
    }
    return ERR_SUCCESS;
}
