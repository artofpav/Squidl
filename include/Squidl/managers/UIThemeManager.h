#pragma once
#include "Squidl/managers/TextureManager.h"
#include "Squidl/managers/UITheme.h"
#include "Squidl/SquidlConfig.h"

namespace Squidl::Managers {
    SQUIDL_API class UIThemeManager {
      public:
        static void setTheme(const UITheme &theme);
        static const UITheme &getTheme();

      private:
        static UITheme currentTheme;
    };
}