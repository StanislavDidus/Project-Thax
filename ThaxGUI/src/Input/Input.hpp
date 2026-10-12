#pragma once

#include "DMG/DMG.hpp"
#include "Graphics/Graphics.hpp"

namespace dmg
{
    inline void HandleKey(dmg::DMG& dmg, const GraphicsContext& graphics_context, const int glfw_key, const JoypadButton joypad_button)
    {
        const int state = glfwGetKey(graphics_context.window, glfw_key);

        if (state == GLFW_PRESS)
            dmg.KeyPressed(joypad_button);
        else if (state == GLFW_RELEASE)
            dmg.KeyReleased(joypad_button);
    }

    inline void HandleInput(const GraphicsContext& graphics_context, dmg::DMG& dmg)
    {
        // D-PAD
        HandleKey(dmg, graphics_context, GLFW_KEY_UP, JoypadButton::DPAD_UP);
        HandleKey(dmg, graphics_context, GLFW_KEY_DOWN, JoypadButton::DPAD_DOWN);
        HandleKey(dmg, graphics_context, GLFW_KEY_RIGHT, JoypadButton::DPAD_RIGHT);
        HandleKey(dmg, graphics_context, GLFW_KEY_LEFT, JoypadButton::DPAD_LEFT);

        // BUTTONS
        HandleKey(dmg, graphics_context, GLFW_KEY_Z, JoypadButton::B);
        HandleKey(dmg, graphics_context, GLFW_KEY_X, JoypadButton::A);
        HandleKey(dmg, graphics_context, GLFW_KEY_ENTER, JoypadButton::START);
        HandleKey(dmg, graphics_context, GLFW_KEY_RIGHT_SHIFT, JoypadButton::SELECT);
    }
}
