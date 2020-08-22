#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    Display *display;
    Window window;
    XEvent event;
    int eventLoopRunOnce = 0;
    int s;

    printf("Starting...\n");
 
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
    XSelectInput(display, window, KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask );
 
    /* map (show) the window */
    XMapWindow(display, window);

    printf("Starting event loop...\n");

    /* event loop */
    while (1)
    {
        XNextEvent(display, &event);
 
        /* keyboard events */
        if (event.type == KeyPress)
        {
            printf( "KeyPress: %x\n", event.xkey.keycode );

            /* exit on ESC key press */
            if ( event.xkey.keycode == 0x09 )
                break;
        }
        else if (event.type == KeyRelease)
        {
            printf( "KeyRelease: %x\n", event.xkey.keycode );
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
    }

    /* close connection to server */
    XCloseDisplay(display);
 
    return 0;
}

