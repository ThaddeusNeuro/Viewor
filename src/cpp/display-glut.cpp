// display.cpp - actually 'C' - this uses GLUT - to be replaced by display-pure.cpp  (OpenGL and XLib)

#include <GL/freeglut.h> // Or <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h> // For exit()
#include <math.h>   // For fabs()

// Global accelerometer data
float g_accelX = 0.0f;
float g_accelY = 0.0f;
float g_accelZ = 0.0f;

// Camera parameters (for easy viewing)
float cameraAngleX = 30.0f;
float cameraAngleY = 30.0f;
float cameraDistance = 5.0f; // Distance from origin

// Function to update accelerometer data
void updateAccelerometer(float x, float y, float z) {
    g_accelX = x;
    g_accelY = y;
    g_accelZ = z;
    glutPostRedisplay(); // Request a redraw
}

// Function to draw the 3D axes and the dynamic vectors
void display() 
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

    // Draw the static reference axes using DisplayList - might not work with OpenGL4
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
    // Normalize acceleration values for display length (adjust 'scale' as needed)
    float displayScale = 0.5f; // Adjust to make the lines visible and responsive

    glLineWidth(2.0f); // Make dynamic lines thicker
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

    // Optional: Draw spheres at the end of vectors to indicate direction
    // For this, you might need a utility like gluSphere or implement your own.
    // For simplicity, I'm omitting sphere drawing in this basic example.
    // If accelX > 0, draw sphere at (g_accelX * displayScale, 0, 0)
    // etc.

    glutSwapBuffers(); // Swap the front and back buffers
}

// Function to handle window reshaping
void reshape(int width, int height) 
{
    if (height == 0) height = 1; // Prevent division by zero
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 0.1, 100.0); // FOV, Aspect, Near, Far
    glMatrixMode(GL_MODELVIEW);
}

// Keyboard input callback
void keyboard(unsigned char key, int x, int y) 
{
    switch (key) {
        case 27: // ESC key
            exit(0);
            break;
        case 'u': cameraAngleX -= 5.0f; break; // Rotate camera up
        case 'd': cameraAngleX += 5.0f; break; // Rotate camera down
        case 'l': cameraAngleY -= 5.0f; break; // Rotate camera left
        case 'r': cameraAngleY += 5.0f; break; // Rotate camera right
        case '+': cameraDistance -= 0.5f; if (cameraDistance < 1.0f) cameraDistance = 1.0f; break;
        case '-': cameraDistance += 0.5f; break;
    }
    glutPostRedisplay();
}

// Idle function (for continuous redraw if data updates)
void idle() 
{
    // Simulate accelerometer data update for demonstration
    // tobe replaced by reading from a sensor
    static float t = 0.0f;
    t += 0.01f;
    // Simple sinusoidal motion for demo
    updateAccelerometer(
        sin(t * 2.0f) * 0.8f,  // X acceleration
        cos(t * 1.5f) * 0.6f,  // Y acceleration
        sin(t * 3.0f) * 0.4f   // Z acceleration
    );

    // If you're polling a sensor, you'd do it here:
    // readSensorData(&x, &y, &z);
    // updateAccelerometer(x, y, z);

    glutPostRedisplay(); // Request a redraw
}

int main(int argc, char **argv) 
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); // Double buffer, RGB color, Depth testing
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D Accelerometer Display");

    // Initialize OpenGL state
    glEnable(GL_DEPTH_TEST); // Enable depth testing for proper 3D rendering
    glClearColor(0.9f, 0.9f, 1.0f, 1.0f); // Light blue background

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(idle); // Set the idle callback

    glutMainLoop(); // Enter the GLUT event processing loop
    return 0;
}