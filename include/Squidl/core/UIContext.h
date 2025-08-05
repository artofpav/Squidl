// UIContext.h
#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include "Squidl/utils/UIRect.h"
#include "Squidl/utils/Point.h"
#include "Squidl/SquidlConfig.h" // For SQUIDL_API
namespace Squidl::Core {
    SQUIDL_API struct UIContext {

        int mouseX = 0;
        int mouseY = 0;
        bool mouseDown = false;
        bool mousePressed = false;
        bool mouseReleased = false;
        int getMouseX(){return mouseX;};
        int getMouseY() { return mouseY; };
        Squidl::Utils::Point getMousePosition() const {
            return {mouseX, mouseY};
        }

          const Uint8 *keyboardState = nullptr;
        int keyboardStateSize = 0;

        void handleEvent(const SDL_Event &e);
        void beginFrame();

        void setSize(int w, int h);
        int getWidth() const;
        int getHeight() const;
        SDL_Rect getRect() { return {0, 0, windowW, windowH}; }

        // Added methods for checking mouse button states
        bool isMouseButtonDown(Uint8 button) const {
            return mouseDown && (button == SDL_BUTTON_LEFT);
        }
        bool isMouseButtonPressed(Uint8 button) const {
            return mousePressed && (button == SDL_BUTTON_LEFT);
        }
        bool isMouseButtonReleased(Uint8 button) const {
            return mouseReleased && (button == SDL_BUTTON_LEFT);
        }

        // Added method for checking keyboard key states
        bool isKeyPressed(SDL_Scancode key) const {
            if (keyboardState && key < keyboardStateSize) {
                return keyboardState[key] != 0;
            }
            return false;
        }

      private:
        int windowW = 1000;
        int windowH = 800;
    };
} // namespace Squidl::Core
