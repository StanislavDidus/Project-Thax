#pragma once

#include "DMG/Utils/Log.hpp"

#define GLFW_INCLUDE_NONE
#include <span>
#include <glad/glad.h>
#include "GLFW/glfw3.h"

namespace dmg
{
    constexpr unsigned int WINDOW_WIDTH = 1080;
    constexpr unsigned int WINDOW_HEIGHT = 720;
    constexpr const char* TITLE = "Game Boy Emulator";

    inline void error_callback(int error, const char* description)
    {
        DMG_ERROR("Error happened in Graphics Context: {}", description);
    }

    inline void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        //if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            //glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    struct GraphicsContext
    {
        GLFWwindow* window = nullptr;
    };

    inline GraphicsContext InitializeGraphicsContext()
    {
        GraphicsContext graphics_context{};

        if (!glfwInit())
        {
            DMG_ERROR("GLFW Initialization failed.");
            throw std::runtime_error{"GLFW Initialization failed."};
        }

        glfwSetErrorCallback(error_callback);

        GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, TITLE, nullptr, nullptr);
        if (!window)
        {
            DMG_ERROR("GLFW Window could not be created.");
            throw std::runtime_error{"GLFW Window could not be created."};
        }

        graphics_context.window = window;

        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        {
            DMG_ERROR("Failed to initialize GLFW Context.");
            throw std::runtime_error{"Failed to initialize GLFW Context."};
        }

        glfwSetKeyCallback(window, input_callback);

        glfwSwapInterval(0);

        DMG_INFO("Graphics Context was created.");

        return graphics_context;
    }

    inline void DestroyGraphicsContext(GraphicsContext& graphics_context)
    {
        glfwDestroyWindow(graphics_context.window);
        glfwTerminate();

        DMG_INFO("Graphics Context was destroyed.");
    }

    struct RGBColor
    {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
    };

    template<std::size_t N>
    std::array<uint8_t, N * 3> ConvertBufferToRGB(const std::array<uint8_t, N>& buffer)
    {
        std::array<uint8_t, N * 3> rgb;

        static RGBColor c00 = {.r = 139,.g = 172, .b = 15};
        static RGBColor c01 = {.r = 48, .g = 98, .b = 48};
        static RGBColor c10 = {.r = 15, .g = 56, .b = 15};
        static RGBColor c11 = {.r = 15, .g = 56, .b = 15};

        for (int i = 0; i < buffer.size(); ++i)
        {
            RGBColor color{};
            if (buffer[i] == 0b00) color = c00;
            else if (buffer[i] == 0b01) color = c01;
            else if (buffer[i] == 0b10) color = c10;
            else if (buffer[i] == 0b11) color = c11;

            rgb[i * 3 + 0] = color.r;
            rgb[i * 3 + 1] = color.g;
            rgb[i * 3 + 2] = color.b;
        }

        return rgb;
    }

    inline void GenerateTextureFromBuffer(unsigned int gl_texture, int texture_width, int texture_height, std::unique_ptr<std::vector<uint8_t>> buffer)
    {
        glBindTexture(GL_TEXTURE_2D, gl_texture);

        std::unique_ptr<std::vector<uint8_t>> rgb = std::make_unique<std::vector<uint8_t>>();
        rgb->resize(buffer->size() * 3);

        static RGBColor c00 = {.r = 139,.g = 172, .b = 15};
        static RGBColor c01 = {.r = 48, .g = 98, .b = 48};
        static RGBColor c10 = {.r = 15, .g = 56, .b = 15};
        static RGBColor c11 = {.r = 15, .g = 56, .b = 15};

        for (int i = 0; i < buffer->size(); ++i)
        {
            RGBColor color{};
            if ((*buffer)[i] == 0b00) color = c00;
            else if ((*buffer)[i] == 0b01) color = c01;
            else if ((*buffer)[i] == 0b10) color = c10;
            else if ((*buffer)[i] == 0b11) color = c11;

            (*rgb)[i * 3 + 0] = color.r;
            (*rgb)[i * 3 + 1] = color.g;
            (*rgb)[i * 3 + 2] = color.b;
        }

        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
            texture_width, texture_height,
            GL_RGB, GL_UNSIGNED_BYTE, rgb->data());
    }

    inline unsigned int GenerateGLTexture(int texture_width, int texture_height)
    {
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
            texture_width, texture_height,
            0, GL_RGB,
            GL_UNSIGNED_BYTE, nullptr);
        glBindTexture(GL_TEXTURE_2D, 0);
        return texture;
    }
}