#pragma once
#include <SDL.h>     // For SDL_Texture (still used for now)
#include <SDL_ttf.h> // For TTF_Font (still used for now)
#include <algorithm> // For std::clamp
#include <memory>    // For std::shared_ptr, std::weak_ptr

#include "Squidl/SquidlConfig.h"     // For SQUIDL_API
#include "Squidl/core/IRenderer.h"   // Include IRenderer.h
#include "Squidl/core/UIAlignment.h" // For HorizontalAlign, VerticalAlign
#include "Squidl/core/UIAnchor.h"    // For UIAnchor
#include "Squidl/core/UIEvent.h"     // <--- ADDED: Include UIEvent.h
#include "Squidl/utils/Color.h"      // Use Squidl::Utils::Color
#include "Squidl/utils/Point.h"      // For Squidl::Utils::Point
#include "Squidl/utils/UIRect.h"     // Use Squidl::Utils::UIRect

// Forward declarations for types used as pointers/references
namespace Squidl::Core {
    class UIContext;
}
namespace Squidl::Elements {
    class Backdrop;
} // namespace Squidl::Elements

namespace Squidl::Base {
    SQUIDL_API class UIElement
        : public std::enable_shared_from_this<
              UIElement> { // <--- ADDED: Inherit from enable_shared_from_this
      public:
        virtual ~UIElement() = default;

        // UIContext now qualified by Squidl::Core namespace
        // Renderer now accepts IRenderer&
        virtual bool update(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) = 0;

        // Changed return type and parameter type to Squidl::Utils::UIRect
        virtual Squidl::Utils::UIRect getRect();
        virtual void setRect(const Squidl::Utils::UIRect &newRect);

        void setPosition(int x, int y) { position = {x, y}; }
        void setPosition(const Squidl::Utils::Point &p) { position = p; }
        Squidl::Utils::Point getPosition() const { return position; }
        Squidl::Utils::UIRect getLocalRect() const;

        virtual void autosize() = 0;
        virtual void applyConstraints() {
            if (rect.w < minW)
                rect.w = minW;
            if (rect.h < minH)
                rect.h = minH;
            if (rect.w > maxW)
                rect.w = maxW;
            if (rect.h > maxH)
                rect.h = maxH;
        };
        void setMinSize(int w, int h) {
            minW = w;
            minH = h;
            applyConstraints();
        }

        void setMaxSize(int w, int h) {
            maxW = w;
            maxH = h;
            applyConstraints();
        }

        virtual void setFont(TTF_Font *f) { font = f; }
        virtual TTF_Font *getFont() { return font; }

        // Changed parameter and return types to Squidl::Utils::Color
        void setBackgroundColor(Squidl::Utils::Color color) { bgColor = color; }
        Squidl::Utils::Color getBackgroundColor() const { return bgColor; }
        void setBackdrop(std::shared_ptr<Squidl::Elements::Backdrop> b) {
            backdrop = std::move(b);
        }
        void setBorderColor(Squidl::Utils::Color color) { borderColor = color; }
        Squidl::Utils::Color getBorderColor() const { return borderColor; }
        void setBorderless(bool value) { borderless = value; }
        bool isBorderless() { return borderless; }

        void setOpacity(float value) {
            opacity = std::clamp(value, 0.0f, 1.0f);
        }
        float getOpacity() const { return opacity; }
        void setBorderOpacity(float value) {
            borderOpacity = std::clamp(value, 0.0f, 1.0f);
        }
        float getBorderOpacity() const { return borderOpacity; }

        void setManagedByLayout(bool value) { managedByLayout = value; }
        bool isManagedByLayout() const { return managedByLayout; }

        // --- NEW: isManagedByChilds parameter ---
        void setManagedByChilds(bool value) { managedByChilds = value; }
        bool isManagedByChilds() const { return managedByChilds; }
        // ----------------------------------------

        void setAnchor(Squidl::Core::UIAnchor a) { anchors = a; }
        Squidl::Core::UIAnchor getAnchor() const { return anchors; }
        void updateAnchoredRect(const Squidl::Utils::UIRect &parentRect);

        void setHorizontalAlign(Squidl::Core::HorizontalAlign align) {
            hAlign = align;
        }
        void setVerticalAlign(Squidl::Core::VerticalAlign align) {
            vAlign = align;
        }
        Squidl::Core::HorizontalAlign getHorizontalAlign() const {
            return hAlign;
        }
        Squidl::Core::VerticalAlign getVerticalAlign() const { return vAlign; }

        void setParent(std::shared_ptr<UIElement> p) { parent = p; }
        std::shared_ptr<UIElement> getParent() const { return parent.lock(); }

        template <typename T> std::shared_ptr<T> getParentOfType() const {
            auto p = parent.lock();
            while (p) {
                auto typed = std::dynamic_pointer_cast<T>(p);
                if (typed)
                    return typed;
                p = p->getParent();
            }
            return nullptr;
        }

        std::shared_ptr<UIElement> getRoot() const {
            auto p = getParent();
            if (!p)
                return nullptr;
            while (auto next = p->getParent())
                p = next;
            return p;
        }

        int index = 0;

        /**
         * @brief Виртуальный метод для обработки событий.
         * Элементы UI переопределяют этот метод для реагирования на события.
         * @param event Событие UI.
         */
        virtual void onEvent(Squidl::Core::UIEvent &event) {
            // Базовая реализация не делает ничего, но позволяет переопределять
            // в дочерних классах. Если событие не было обработано этим
            // элементом, оно может быть передано дальше.
        }

      protected:
        // Renderer now accepts IRenderer&
        virtual void updateBackdrop(Squidl::Core::UIContext &ctx,
                                    Squidl::Core::IRenderer &renderer) = 0;

        std::weak_ptr<UIElement> parent;

        Squidl::Utils::UIRect rect;
        Squidl::Utils::Point position = {0, 0};
        int minW = 0, minH = 0;
        int maxW = 10000, maxH = 10000;

        TTF_Font *font = nullptr;
        Squidl::Utils::Color bgColor = {100, 100, 100, 255};
        Squidl::Utils::Color borderColor = {0, 0, 0, 255};

        std::shared_ptr<Squidl::Elements::Backdrop> backdrop;

        float opacity = 1.0f;
        float borderOpacity = 1.0f;
        bool borderless = false;

        // ---------------- Paddings ------------------
        Squidl::Core::Padding padding = 5;

        // -----Parent/Childs Controlable -------------
        bool managedByLayout = false;
        bool managedByChilds = false;

        // ------------Anchors-------------------------
        Squidl::Core::UIAnchor anchors = Squidl::Core::UIAnchor::None;
        int anchorOffsetLeft = 0;
        int anchorOffsetRight = 0;
        int anchorOffsetTop = 0;
        int anchorOffsetBottom = 0;
        bool anchorOffsetsInitialized = false;

        Squidl::Core::HorizontalAlign hAlign =
            Squidl::Core::HorizontalAlign::Left;
        Squidl::Core::VerticalAlign vAlign = Squidl::Core::VerticalAlign::Top;
    };
} // namespace Squidl::Base