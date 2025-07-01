// display-pure.cpp - this is actually 'C' - but will evolve to C++ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h> // For memset
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <math.h> // For sin, cos, fabs

// Global accelerometer data
float g_accelX = 0.0f;
float g_accelY = 0.0f;
float g_accelZ = 0.0f;

// Camera parameters
float cameraAngleX = 30.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 5.0f;

// X11 and GLX variables
Display *dpy;
Window win;
GLXContext glc;
XVisualInfo *vi; // Visual information for the window

// Function to update accelerometer data
void updateAccelerometer(float x, float y, float z) {
    g_accelX = x;
    g_accelY = y;
    g_accelZ = z;
    // In a real GLX loop, you'd trigger a redraw here, usually by setting a flag
    // and calling XSync(dpy, False) or similar to ensure event processing.
}

// Function to draw the 3D axes and the dynamic vectors
void draw_scene() 
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity(); // Reset the model-view matrix

    // Position and orient the camera
    gluLookAt(
        cameraDistance * sin(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f), // Eye X
        cameraDistance * sin(cameraAngleX * M_PI / 180.0f),                                   // Eye Y
        cameraDistance * cos(cameraAngleY * M_PI / 180.0f) * cos(cameraAngleX * M_PI / 180.0f), // Eye Z
        0.0, 0.0, 0.0,   // Look at point (origin)
        0.0, 1.0, 0.0    // Up vector
    );

    // Draw the static reference axes
    glBegin(GL_LINES);
        // X-axis (Red)
        glColor3f(1.0f, 0.0f, 0.0f); 
        glVertex3f(-2.0f, 0.0f, 0.0f);
        glVertex3f(2.0f, 0.0f, 0.0f);

        // Y-axis (Green)
        glColor3f(0.0f, 1.0f, 0.0f); 
        glVertex3f(0.0f, -2.0f, 0.0f);
        glVertex3f(0.0f, 2.0f, 0.0f);

        // Z-axis (Blue)
        glColor3f(0.0f, 0.0f, 1.0f); 
        glVertex3f(0.0f, 0.0f, -2.0f);
        glVertex3f(0.0f, 0.0f, 2.0f);
    glEnd();

    // Draw the dynamic accelerometer vectors
    float displayScale = 0.5f; // Adjust to control sensitivity

    glLineWidth(3.0f); // Make dynamic lines thicker
    glBegin(GL_LINES);
        // X-acceleration vector (e.g., Magenta)
        glColor3f(1.0f, 0.0f, 1.0f); // Magenta
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(g_accelX * displayScale, 0.0f, 0.0f);

        // Y-acceleration vector (e.g., Cyan)
        glColor3f(0.0f, 1.0f, 1.0f); // Cyan
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, g_accelY * displayScale, 0.0f);

        // Z-acceleration vector (e.g., Yellow)
        glColor3f(1.0f, 1.0f, 0.0f); // Yellow
        glVertex3f(0.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, g_accelZ * displayScale);
    glEnd();
    glLineWidth(1.0f); // Reset line width

    glXSwapBuffers(dpy, win); // Swap the front and back buffers
}

// Function to handle window reshaping
void reshape_window(int width, int height) 
{
    if (height == 0) height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

// Initialize X11 and GLX
void init_x11_glx()
 {
    XSetWindowAttributes swa;
    Colormap cmap;
    Window root;

    // 1. Open a display
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL) 
    {
        fprintf(stderr, "Could not open display\n");
        exit(1);
    }
    root = DefaultRootWindow(dpy);

    // 2. Choose a GLX visual that supports OpenGL
    GLint att[] = {
        GLX_RGBA,
        GLX_DEPTH_SIZE, 24,
        GLX_DOUBLEBUFFER,
        None
    };

    vi = glXChooseVisual(dpy, 0, att);
    if (vi == NULL) {
        fprintf(stderr, "No appropriate visual found\n");
        exit(1);
    }
    printf("Visual %p selected\n", (void *)vi->visualid);

    // 3. Create a colormap for the window
    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

    // 4. Set window attributes
    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask; // Important for events!
    swa.background_pixmap = None; // Transparent background

    // 5. Create the X11 window
    win = XCreateWindow(
        dpy,
        root,
        0, 0,       // X, Y position
        800, 600,   // Width, Height
        0,          // Border width
        vi->depth,  // Depth
        InputOutput,// Class
        vi->visual, // Visual
        CWColormap | CWEventMask | CWBackPixmap, // Value mask
        &swa        // Attributes
    );

    // Set window title
    XStoreName(dpy, win, "Pure OpenGL Accelerometer Display");

    // Map the window (make it visible)
    XMapWindow(dpy, win);

    // 6. Create an OpenGL rendering context
    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE); // Last arg: direct rendering (True/False)
    if (glc == NULL) {
        fprintf(stderr, "Failed to create OpenGL context\n");
        exit(1);
    }

    // 7. Make the context current (important for OpenGL calls)
    glXMakeCurrent(dpy, win, glc);

    // Initialize OpenGL rendering settings
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.9f, 0.9f, 1.0f, 1.0f); // Light blue background

    // Get initial window size and set projection
    XWindowAttributes gwa;
    XGetWindowAttributes(dpy, win, &gwa);
    reshape_window(gwa.width, gwa.height);
}

// Clean up X11 and GLX resources
void term_x11_glx() 
{
    glXMakeCurrent(dpy, None, NULL); // Release context
    glXDestroyContext(dpy, glc);
    XDestroyWindow(dpy, win);
    XFreeColormap(dpy, XDefaultColormap(dpy, DefaultScreen(dpy))); // Free default colormap
    XCloseDisplay(dpy);
}


// Main event loop
int main(int argc, char **argv) 
{
    init_x11_glx(); // Setup X11 window and OpenGL context

    XEvent xev;
    long last_update_time = 0; // For simulated data update rate

    while (1) 
    {
        // Poll for X events
        while (XPending(dpy) > 0)
         {
            XNextEvent(dpy, &xev);
            switch (xev.type) 
            {
                case Expose:
                    // Window exposed (e.g., resized, uncovered)
                    // Get current window size and redraw
                    XWindowAttributes gwa;
                    XGetWindowAttributes(dpy, win, &gwa);
                    reshape_window(gwa.width, gwa.height);
                    draw_scene();
                    break;
                case ConfigureNotify:
                    // Window configured (e.g., resized)
                    if (xev.xconfigure.width != 0 && xev.xconfigure.height != 0) {
                        reshape_window(xev.xconfigure.width, xev.xconfigure.height);
                    }
                    break;
                case KeyPress: 
                    {
                    KeySym key = XLookupKeysym(&xev.xkey, 0);
                    switch (key) 
                        {
                        case XK_Escape: // ESC key
                            cleanup_x11_glx();
                            exit(0);
                            break;
                        case XK_w: cameraAngleX -= 5.0f; break;
                        case XK_s: cameraAngleX += 5.0f; break;
                        case XK_a: cameraAngleY -= 5.0f; break;
                        case XK_d: cameraAngleY += 5.0f; break;
                        case XK_plus: // '+'
                            cameraDistance -= 0.5f;
                            if (cameraDistance < 1.0f) cameraDistance = 1.0f;
                            break;
                        case XK_minus: // '-'
                            cameraDistance += 0.5f;
                            break;
                        }
                    break;
                    }
                case ClientMessage:
                    // Handle window close button (WM_DELETE_WINDOW protocol)
                    if (xev.xclient.data.l[0] == XInternAtom(dpy, "WM_DELETE_WINDOW", False)) 
                    {
                        term_x11_glx();
                        exit(0);
                    }
                    break;
            }
        }

        // --- Simulate accelerometer data update (replace with actual sensor reading) ---
        long current_time_ms = (long)(XNextEvent(dpy, &xev), (XPending(dpy)>0 ? (XNextEvent(dpy, &xev), 0) : 0), (xev.xkey.time / 1000.0 * 1000.0)); // Crude way to get time, usually use a timer
        if (current_time_ms - last_update_time > 16) { // Update roughly 60 times per second (1000ms / 60fps ~ 16ms)
            static float t = 0.0f;
            t += 0.01f;
            updateAccelerometer(
                sin(t * 2.0f) * 0.8f,
                cos(t * 1.5f) * 0.6f,
                sin(t * 3.0f) * 0.4f
            );
            last_update_time = current_time_ms;
        }
        // --- End of simulation ---

        draw_scene(); // Always redraw
    }

    return 0;
}