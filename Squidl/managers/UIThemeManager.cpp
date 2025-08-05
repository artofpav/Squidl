#include "Squidl/managers/UIThemeManager.h"

namespace Squidl::Managers {
    UITheme UIThemeManager::currentTheme = UITheme::Dark();

    void UIThemeManager::setTheme(const UITheme &theme) {
        currentTheme = theme;
    }

    const UITheme &UIThemeManager::getTheme() { return currentTheme; }
}
