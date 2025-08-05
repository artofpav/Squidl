#ifndef SQUIDL_H
#define SQUIDL_H

/**
 * @file Squidl.h
 * @brief Main include file for the Squidl UI library.
 *
 * @author ArtOfPav
 *
 * This file provides a convenient way to include all public headers
 * of the Squidl UI library with a single include statement.
 */

// Include library configuration
#include "SquidlConfig.h"

// --- Core Components ---
#include "Squidl/core/EventDispatcher.h"
#include "Squidl/core/UIAlignment.h"
#include "Squidl/core/UIAnchor.h"
#include "Squidl/core/UIContext.h"
#include "Squidl/core/UIEvent.h"
#include "Squidl/core/UIManager.h"
#include "Squidl/core/IRenderer.h"

// --- Base Classes & Interfaces ---
#include "Squidl/base/EventListener.h"
#include "Squidl/base/IClickable.h"
#include "Squidl/base/IDrawable.h"
#include "Squidl/base/UIElement.h"

// --- UI Elements ---
#include "Squidl/elements/Backdrop.h"
#include "Squidl/elements/Button.h"
#include "Squidl/elements/Input.h"
#include "Squidl/elements/ToggleSwitch.h"
#include "Squidl/elements/Label.h"
// Add new elements here as you create them, e.g.:
// #include "Squidl/elements/Checkbox.h"
// #include "Squidl/elements/Slider.h"

// --- Layouts ---
#include "Squidl/layouts/GridLayout.h"
#include "Squidl/layouts/HBoxLayout.h"
#include "Squidl/layouts/Layout.h"
#include "Squidl/layouts/VBoxLayout.h"

// --- Managers ---
#include "Squidl/managers/FontManager.h"
#include "Squidl/managers/ResourceManager.h"
#include "Squidl/managers/TextureManager.h"
#include "Squidl/managers/UITheme.h"
#include "Squidl/managers/UIThemeManager.h"

// --- Renderers ---
#include "Squidl/renderers/SDL2Renderer.h"

// --- Utilities ---
#include "Squidl/utils/Color.h"
#include "Squidl/utils/UIRect.h" 
#include "Squidl/utils/Point.h"
#include "Squidl/utils/Logger.h"
#include "Squidl/utils/Timer.h"

// Note: Editor-specific headers are generally not included in the main
// library include, as they are for a separate tool/application.
// If you want to allow integration of editor components, you might include them
// conditionally or in a separate editor-specific header, e.g.,
// "SquidlEditor.h".

#endif // SQUIDL_H