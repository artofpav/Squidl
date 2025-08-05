#ifndef SQUIDL_CONFIG_H
#define SQUIDL_CONFIG_H

/**
 * @file SquidlConfig.h
 * @brief Configuration file for the Squidl UI library.
 *
 * This file defines general configuration macros, version information,
 * and other compile-time settings for the Squidl UI library.
 */

// Library Version Information
#define SQUIDL_VERSION_MAJOR 0
#define SQUIDL_VERSION_MINOR 1
#define SQUIDL_VERSION_PATCH 0
#define SQUIDL_VERSION_STRING "0.1.0"

// Export/Import Macros for Windows DLLs
#ifdef _WIN32
    #ifdef SQUIDL_EXPORTS
        // If compiling the Squidl library itself
        #define SQUIDL_API __declspec(dllexport)
    #else
        // If using the Squidl library as a client
        #define SQUIDL_API __declspec(dllimport)
    #endif
#else
    // For non-Windows platforms (Linux, macOS, etc.)
    #define SQUIDL_API
#endif

// Define common namespaces for convenience
namespace Squidl {
    // Top-level namespace for the entire library
}

namespace Squidl::Core {
    // Core components: UIContext, UIManager, UIRenderer, EventDispatcher
}

namespace Squidl::Elements {
    // UI elements: Button, Label, Input, Checkbox, Slider, etc.
}

namespace Squidl::Layouts {
    // Layout managers: GridLayout, HBoxLayout, VBoxLayout
}

namespace Squidl::Managers {
    // Resource managers: ResourceManager, FontManager, TextureManager, UIThemeManager
}

namespace Squidl::Base {
    // Base classes and interfaces: UIElement, IClickable, IDrawable, EventListener
}

namespace Squidl::Utils {
    // Utility classes: Color, Rect, Logger
}

namespace Squidl::Editor {
    // UI Editor specific classes
}


#endif // SQUIDL_CONFIG_H