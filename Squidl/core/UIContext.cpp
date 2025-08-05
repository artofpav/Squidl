// UIContext.cpp
#include "Squidl/core/UIContext.h"
#include <SDL.h> // For SDL_GetKeyboardState

namespace Squidl::Core {

    void UIContext::beginFrame() {
        mousePressed = false;
        mouseReleased = false;
        // Обновляем состояние клавиатуры в начале каждого кадра
        keyboardState = SDL_GetKeyboardState(&keyboardStateSize);
    }

    void UIContext::handleEvent(const SDL_Event &e) {
        if (e.type == SDL_MOUSEMOTION) {
            mouseX = e.motion.x;
            mouseY = e.motion.y;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN &&
            e.button.button == SDL_BUTTON_LEFT) {
            mouseDown = true;
            mousePressed = true;
        }
        if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
            mouseDown = false;
            mouseReleased = true;
        }
        // События клавиатуры обрабатываются SDL_GetKeyboardState,
        // поэтому здесь не требуется специфическая обработка для isKeyPressed.
        // Однако, если бы мы хотели отслеживать однократные нажатия (key
        // pressed/released this frame), то это делалось бы здесь.
    }

    void UIContext::setSize(int w, int h) {
        windowW = w;
        windowH = h;
    }

    int UIContext::getWidth() const { return windowW; }
    int UIContext::getHeight() const { return windowH; }

} // namespace Squidl::Core
