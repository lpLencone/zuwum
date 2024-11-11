#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// clang-format off
#define eprintf(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
#define panic(fmt, ...) do { eprintf(fmt __VA_OPT__(,) __VA_ARGS__); exit(1); } while (0);
#define expect(cond, msg) do { if (!(cond)) { panic("\"%s\": " msg "\n", #cond); } } while (0);
// clang-format on

int main(void)
{
    Display *display;
    Window window;
    int screen;
    Atom wm_del_msg;
    {
        display = XOpenDisplay(NULL);
        expect(display != NULL, "Could not open display");
        screen = XDefaultScreen(display);
        // int depth = XDefaultDepth(display, screen);
        Window root_window = RootWindow(display, screen);
        window = XCreateSimpleWindow(
                display, root_window, 100, 10, WINDOW_WIDTH, WINDOW_HEIGHT, 5,
                XBlackPixel(display, screen), XWhitePixel(display, screen));
        XSizeHints hints = {
            .min_width = WINDOW_WIDTH,
            .max_width = WINDOW_WIDTH,
            .min_height = WINDOW_HEIGHT,
            .max_height = WINDOW_HEIGHT,
        };
        XSetStandardProperties(
                display, window, "Simple Window", "window", 0, NULL, 0, &hints);
        XSelectInput(
                display, window,
                ButtonPressMask | KeyPressMask | PointerMotionMask | ExposureMask);
        XMapWindow(display, window);
        wm_del_msg = XInternAtom(display, "WM_DELETE_WINDOW", false);
        XSetWMProtocols(display, window, &wm_del_msg, 1);
    }

    while (true) {
        /* Handle Event */ {
            XEvent xev;
            XNextEvent(display, &xev);

            switch (xev.type) {
                case Expose:
                    XDrawString(
                            display, window, DefaultGC(display, screen), 10, 50,
                            "Fuck all", strlen("Fuck all"));
                    break;

                case ClientMessage:
                    if ((unsigned long) xev.xclient.data.l[0] == wm_del_msg) {
                        goto die;
                    }
                    break;

                case KeyPress:
                    KeySym key = XLookupKeysym((XKeyEvent *) &xev, 0);
                    if (key != 0) {
                        printf("Keyboard event\n");
                    }
                    break;

                case ButtonPressMask:
                case PointerMotionMask:
                    printf("Mouse event\n");
                    break;

                default:
            }
        }
    }

die:
    XDestroyWindow(display, window);
    XCloseDisplay(display);

    return 0;
}
