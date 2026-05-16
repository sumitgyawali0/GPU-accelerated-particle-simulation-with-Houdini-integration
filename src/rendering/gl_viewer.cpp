#ifdef RAYMARCH_USE_GLFW

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>

#include "rendering/gl_viewer.h"

#include <iostream>

namespace
{

void uploadTexture(const Framebuffer& framebuffer, GLuint texture)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, framebuffer.width(), framebuffer.height(), 0, GL_RGB,
                 GL_UNSIGNED_BYTE, framebuffer.pixels().data());
}

void drawFullscreenQuad()
{
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f);
    glVertex2f(-1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex2f(1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex2f(1.0f, 1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex2f(-1.0f, 1.0f);
    glEnd();
}

} // namespace

bool runInteractiveViewer(const Framebuffer& framebuffer, const RenderSettings& settings)
{
    (void)settings;

    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    GLFWwindow* window =
        glfwCreateWindow(framebuffer.width(), framebuffer.height(), "CUDA Coursework — Ray March", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    GLuint texture = 0;
    glGenTextures(1, &texture);
    glEnable(GL_TEXTURE_2D);

    while (!glfwWindowShouldClose(window))
    {
        uploadTexture(framebuffer, texture);

        int winW = 0;
        int winH = 0;
        glfwGetFramebufferSize(window, &winW, &winH);
        glViewport(0, 0, winW, winH);

        glClearColor(0.02f, 0.03f, 0.06f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindTexture(GL_TEXTURE_2D, texture);
        drawFullscreenQuad();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteTextures(1, &texture);
    glfwDestroyWindow(window);
    glfwTerminate();
    return true;
}

#else

bool runInteractiveViewer(const Framebuffer& framebuffer, const RenderSettings& settings)
{
    (void)framebuffer;
    (void)settings;
    return false;
}

#endif
