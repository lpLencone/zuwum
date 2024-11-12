#include <GL/glu.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

// clang-format off

#define eprintf(fmt, ...) fprintf(stderr, fmt __VA_OPT__(,) __VA_ARGS__)
#define panic(fmt, ...) do { eprintf(fmt __VA_OPT__(,) __VA_ARGS__); exit(1); } while (0)
#define panic_if(cond, fmt, ...) do { if (cond) { panic(fmt __VA_OPT__(,) __VA_ARGS__); } } while (0)

#define expect(cond, fmt, ...) do {                                \
    panic_if(!(cond), "%s:%d: Condition \"%s\" failed: " fmt "\n", \
             __FILE__, __LINE__, #cond __VA_OPT__(,) __VA_ARGS__); \
} while (0)

// clang-format on

int main(void)
{
    Display *display;
    Window window;
    int screen;
    GLXContext context;

    /* Initialize */ {
        display = XOpenDisplay(NULL);
        expect(display != NULL, "Could not open display");
        screen = DefaultScreen(display);
        int major, minor;
        expect(glXQueryVersion(display, &major, &minor), "Failed to query glx version");
        expect((major == 1 && minor >= 3) || major > 1, "Expected version >= 1.3");
        printf("GLX version %d.%d\n", major, minor);

        int fbc_count;
        int const attrib_list[] = {
            // clang-format off
            GLX_X_RENDERABLE    , True,
            GLX_DRAWABLE_TYPE   , GLX_WINDOW_BIT,
            GLX_RENDER_TYPE     , GLX_RGBA_BIT,
            GLX_X_VISUAL_TYPE   , GLX_TRUE_COLOR,
            GLX_RED_SIZE        , 8,
            GLX_GREEN_SIZE      , 8,
            GLX_BLUE_SIZE       , 8,
            GLX_ALPHA_SIZE      , 8,
            GLX_DEPTH_SIZE      , 24,
            GLX_STENCIL_SIZE    , 8,
            GLX_DOUBLEBUFFER    , True,
            None
            // clang-format on
        };
        GLXFBConfig *fbc = glXChooseFBConfig(display, screen, attrib_list, &fbc_count);
        expect(fbc != NULL, "Faield to retrieve framebuffer configuration.");
        printf("Framebuffer Config count: %d\n", fbc_count);
        printf("GLX extension: %s\n", glXQueryExtensionsString(display, screen));

        GLint attr_list[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
        Window root = DefaultRootWindow(display);
        XVisualInfo *visual_info = glXChooseVisual(display, 0, attr_list);
        expect(visual_info != NULL, "No appropriate visual found.");
        printf("Visual %zu selected\n", visual_info->visualid);
        Colormap color_map =
                XCreateColormap(display, root, visual_info->visual, AllocNone);
        XSetWindowAttributes swattr = {
            .colormap = color_map,
            .event_mask = ExposureMask | KeyPressMask,
        };
        window = XCreateWindow(
                display, root, 0, 0, 800, 600, 0, visual_info->depth, InputOutput,
                visual_info->visual, CWColormap | CWEventMask, &swattr);

        XMapWindow(display, window);
        XStoreName(display, window, "Wordpress e-commerce site :D");
        context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
        glXMakeCurrent(display, window, context);

        glEnable(GL_DEPTH_TEST);
    }

    XWindowAttributes window_attrs;

    while (true) {
        XEvent xev;
        XNextEvent(display, &xev);
        if (xev.xany.window != window) {
            printf("Not this window's event wtf\n");
            continue;
        }

        switch (xev.type) {
            case Expose:
                XGetWindowAttributes(display, window, &window_attrs);
                glViewport(0, 0, window_attrs.width, window_attrs.height);

                glClearColor(1, 1, 1, 1);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                glOrtho(-1, 1, -1, 1, 1, 20);

                glMatrixMode(GL_MODELVIEW);
                glLoadIdentity();
                gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);

                glBegin(GL_QUADS);
                {
                    glColor3f(1, 0, 0);
                    glVertex3f(-0.5, -0.5, 0);
                    glColor3f(0, 1, 0);
                    glVertex3f(0.5, -0.5, 0);
                    glColor3f(0, 0, 1);
                    glVertex3f(0.5, 0.5, 0);
                    glColor3f(1, 1, 0);
                    glVertex3f(-0.5, 0.5, 0);
                }
                glEnd();

                glXSwapBuffers(display, window);
                break;
            case KeyPress:
            default:
                goto die;
        }
    }

die:
    glXMakeCurrent(display, None, NULL);
    glXDestroyContext(display, context);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
    return 0;
}
