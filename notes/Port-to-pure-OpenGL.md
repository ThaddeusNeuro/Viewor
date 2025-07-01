Port-to-pure-OpnGL.notes

Window System Integration: OpenGL itself is just a rendering API. It doesn't create windows or handle user input. On Linux, this is handled by the X Window System (X11) through its Xlib library.

GLX: This is the "OpenGL Extension to the X Window System." It provides the bridge between Xlib and OpenGL, allowing you to create an OpenGL rendering context and bind it to an X11 window.

Event Loop: You have to manage the X11 event loop yourself (keyboard, mouse, window resize, exposure events).

Modern OpenGL (Optional but Recommended): While you can use the fixed-function pipeline as in the GLUT example, moving to "pure" OpenGL often implies using modern, shader-based OpenGL. This involves loading extension function pointers (e.g., with GLEW or GLAD), writing GLSL shaders, and using Vertex Buffer Objects (VBOs) and Vertex Array Objects (VAOs). For this example, I'll stick to the fixed-function pipeline for simplicity, as modern OpenGL is a whole other beast of complexity on top of the windowing.

Key Libraries (instead of GL/freeglut.h):

X11/Xlib.h: For general X Window System operations (opening display, creating windows, handling events).

X11/Xutil.h: For utility functions related to X11.

GL/gl.h: Core OpenGL functions.

GL/glu.h: OpenGL Utility Library (for gluLookAt, gluPerspective - these are convenience functions, not "pure" OpenGL in the sense of being direct GPU calls, but they simplify matrix setup. You could replace them with manual matrix math).

GL/glx.h: For GLX functions (creating OpenGL contexts, swapping buffers).