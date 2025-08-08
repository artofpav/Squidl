#include <SDL.h>
#include <SDL_ttf.h>
#include <cmath> // For std::sqrt
#include <iostream>
#include <locale> // For std::setlocale
#include <memory> // For std::unique_ptr

#include "Squidl/Squidl.h"

using namespace Squidl::Core;
using namespace Squidl::Elements;
using namespace Squidl::Layouts;
using namespace Squidl::Managers;
using namespace Squidl::Utils; // For Color and UIRect

int main() {

    std::setlocale(LC_ALL, "en_US.UTF-8");

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "TTF_Init failed: " << TTF_GetError() << "\n";
        return 1;
    }

    int w = 1024, h = 768; // Increased window size for more layouts
    SDL_Window *window =
        SDL_CreateWindow("Squidl UI Layout Tests", // Updated window title
                         SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h,
                         SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *renderer =
        SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    // Load font
    TTF_Font *font = TTF_OpenFont("assets/Roboto-Regular.ttf", 18);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << "\n";
        return 1;
    }

    Squidl::Utils::Logger::setLogLevel(Squidl::Utils::LogLevel::Debug);

    SQUIDL_LOG_INFO << u8"Программа запущена.";
    Squidl::Utils::Timer timer;
    timer.start();
    // Simulate some work
    for (int i = 0; i < 1000000; ++i) {
        double d = std::sqrt(i * i + 1.0);
    }
    Uint32 elapsed_time = timer.getTicks();
    timer.stop();
    SQUIDL_LOG_INFO << u8"Прошло времени: " << elapsed_time << u8" мс.";

    UIContext ctx;     // Create UI context with default constructor
    ctx.setSize(w, h); // Set initial size

    auto backdrop = std::make_shared<Backdrop>();
    backdrop->setRect({0, 0, w, h});
    backdrop->setBorderless(true);
    backdrop->setTextureFromFile(renderer, "assets/green-bkg.jpg");
    SQUIDL_LOG_INFO << u8"Контекст Создан.";
    // --- IRenderer setup ---
    std::unique_ptr<Squidl::Core::IRenderer> uiRenderer =
        std::make_unique<Squidl::Renderers::SDL2Renderer>(renderer);

    // --- Main Layout (Vertical Box Layout to hold other layouts) ---

    auto mainLayout = std::make_shared<VBoxLayout>();
    // Main layout occupies most of the window
    mainLayout->setRect({50, 50, w - 100, h - 200});

    mainLayout->setBackgroundColor(Color(30, 30, 30, 5)); // Dark background
    mainLayout->setBorderColor({30, 30, 30, 30});
    mainLayout->setBorderless(false); //(Color::White());
    mainLayout->setManagedByChilds(true);
    mainLayout->setPadding(20);
    mainLayout->setSpacing(15);
    mainLayout->setOpacity(0.4f);
    // mainLayout->setBackdrop(layoutBackdrop);
    mainLayout->setHorizontalAlign(Squidl::Core::HorizontalAlign::Stretch);
    SQUIDL_LOG_INFO << u8"mainLayout создан.";

    // --- Test 1: Vertical Box Layout ---

    auto vTestLayout = std::make_shared<VBoxLayout>();
    vTestLayout->autosize();
    vTestLayout->setBackgroundColor(
        Color(30, 35, 50, 10)); // Slightly lighter background
    vTestLayout->setBorderless(true);
    vTestLayout->setPadding(5);
    vTestLayout->setSpacing(1);
    vTestLayout->setOpacity(1);
    vTestLayout->setManagedByChilds(true);
    vTestLayout->setRect({50, 50, w - 200, 300});
    vTestLayout->setHorizontalAlign(Squidl::Core::HorizontalAlign::Left);
    auto label1 =
        std::make_shared<Label>(u8"  Вертикальный Макет", 0, 0, 300, 50, font);
    label1->setBackgroundColor({30, 35, 50, 70});
    label1->setHorizontalAlign(Squidl::Core::HorizontalAlign::Center);
    label1->setTextColor({255, 255, 255, 200});
    vTestLayout->add(label1);

    // Создаем кнопки и устанавливаем их onClick callback
    auto button1 = std::make_shared<Button>(u8"Latest", 0, 0, 300, 50, font);
    button1->setBackgroundColor({30, 35, 50, 120});
    button1->setBorderless(true);
    button1->setHoveredColor({60, 70, 100, 120});
    button1->getLabel()->setTextColor({255, 255, 255, 200});
    button1->onClick = [button1]() {
        SQUIDL_LOG_INFO << "Нажата кнопка: " << button1->getLabelText();
    };
    vTestLayout->add(button1);

    auto input1 =
        std::make_shared<Input>(u8"Leave a message...", 0, 0, 300, 50, font);
    input1->setBackgroundColor({30, 35, 50, 70});
    input1->setBorderless(true);

    vTestLayout->add(input1);

    auto button2 =
        std::make_shared<Button>(u8"Send Message", 0, 0, 300, 40, font);
    button2->setBackgroundColor({30, 35, 50, 120});
    button2->setBorderless(true);
    button2->setHoveredColor({60, 70, 100, 120});
    button2->getLabel()->setTextColor({255, 255, 255, 200});
    button2->onClick = [button2]() {
        SQUIDL_LOG_INFO << "Нажата кнопка: " << button2->getLabelText();
    };
    vTestLayout->add(button2);
    mainLayout->add(vTestLayout);

    // --- Test 2: Horizontal Box Layout ---
    auto hTestLayout = std::make_shared<HBoxLayout>();
    hTestLayout->setBackgroundColor(Color(70, 70, 70, 5));
    // hTestLayout->setBorderColor(Color::Green());
    hTestLayout->setPadding(10);
    hTestLayout->setSpacing(8);
    hTestLayout->setManagedByChilds(true);
    hTestLayout->add(
        std::make_shared<Label>(u8"Горизонтальный Макет", 0, 0, 0, 0, font));

    auto buttonA = std::make_shared<Button>(u8"Кнопка A", 0, 0, 120, 30, font);
    buttonA->onClick = [buttonA]() {
        SQUIDL_LOG_INFO << "Нажата кнопка : " << buttonA->getLabelText();
    };

    hTestLayout->add(buttonA);

    hTestLayout->add(std::make_shared<Input>(u8"Ввод A", 0, 0, 120, 30, font));

    auto buttonB = std::make_shared<Button>(u8"Кнопка B", 0, 0, 120, 30, font);
    buttonB->onClick = [buttonB]() {
        SQUIDL_LOG_INFO << "Нажата кнопка : " << buttonB->getLabelText();
    };
    hTestLayout->add(buttonB);

    hTestLayout->setHorizontalAlign(Squidl::Core::HorizontalAlign::Stretch);
    SQUIDL_LOG_INFO << u8"HLayout создан.";
    mainLayout->add(hTestLayout);

    // --- Test 3: Grid Layout ---
    auto gridTestLayout = std::make_shared<GridLayout>(
        3, 5, 5); // 3 columns, 5 spacing, 8 padding
    gridTestLayout->setRect({50, 50, 700, 200});
    // gridTestLayout->setBackgroundColor(Color(70, 70, 70, 200));
    gridTestLayout->setBorderColor(Color::Red());
    gridTestLayout->setPadding(10);
    gridTestLayout->setSpacing(10);
    gridTestLayout->setManagedByChilds(true);
    gridTestLayout->add(std::make_shared<Label>(u8"Сетка", 0, 0, 0, 0, font));
    gridTestLayout->add(std::make_shared<Input>(u8"Имя", 0, 0, 100, 30, font));
    gridTestLayout->add(
        std::make_shared<Input>(u8"Фамилия", 0, 0, 100, 30, font));

    auto buttonOK = std::make_shared<Button>(u8"ОК", 0, 0, 80, 30, font);
    buttonOK->onClick = [buttonOK]() {
        SQUIDL_LOG_INFO << "Нажата кнопка: " << buttonOK->getLabelText();
    };
    gridTestLayout->add(buttonOK);

    auto buttonCancel =
        std::make_shared<Button>(u8"Отмена", 0, 0, 80, 30, font);
    buttonCancel->onClick = [buttonCancel]() {
        SQUIDL_LOG_INFO << "Нажата кнопка: " << buttonCancel->getLabelText();
    };
    gridTestLayout->add(buttonCancel);

    SQUIDL_LOG_INFO << u8"gridLayout создан.";
    mainLayout->add(gridTestLayout);

    // --- Test 4: Toggle Switch ---
    auto toggleLayout = std::make_shared<HBoxLayout>();
    toggleLayout->setManagedByChilds(true);
    toggleLayout->add(
        std::make_shared<Label>(u8"Переключатель", 0, 0, 0, 0, font));

    auto toggleOn = std::make_shared<ToggleSwitch>(0, 0, 50, 30, true);
    toggleOn->onStateChange = [](bool isOn) {
        SQUIDL_LOG_INFO << "Переключатель " << (isOn ? "включен" : "выключен");
    };
    auto checkbox =
        std::make_shared<Checkbox>(u8"Checkbox", 0, 0, 20, false, font);
    checkbox->setHorizontalAlign(Squidl::Core::HorizontalAlign::Left);
    checkbox->setVerticalAlign(Squidl::Core::VerticalAlign::Center);
    toggleLayout->add(toggleOn);
    toggleLayout->add(checkbox);
    toggleLayout->setBackgroundColor(Color(30, 30, 30, 5));
    toggleLayout->setHorizontalAlign(Squidl::Core::HorizontalAlign::Stretch);
    toggleLayout->setVerticalAlign(Squidl::Core::VerticalAlign::Center);
    mainLayout->add(toggleLayout);
    // --- End Test 4 --- // */

    //*/
    // Set initial size for main layout and trigger autosize for
    // all children
    mainLayout->autosize();

    // Инициализация UIManager
    Squidl::Core::UIManager uiManager;
    uiManager.init(window, renderer,
                   mainLayout); // Передаем mainLayout как корневой элемент

    bool running = true;
    while (running) {
        ctx.beginFrame(); // Reset temporary flags (pressed, released)

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;

            // Передаем все события SDL в UIManager для обработки
            uiManager.handleSDLEvent(event);

            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                int newW = event.window.data1;
                int newH = event.window.data2;

                // Update UIContext, backdrop, and mainLayout size
                SDL_GetWindowSize(window, &w, &h);
                ctx.setSize(w, h);
                backdrop->setRect({0, 0, w, h});
                mainLayout->setRect(
                    {50, 50, w - 100,
                     h - 100}); // Adjust main layout to new window size
            }
        }

        // Clear screen using UIRenderer
        uiRenderer->clearScreen(Color(20, 20, 20, 255));
        uiRenderer->render(backdrop, ctx); // Render backdrop
        // Render all UI elements through UIRenderer
        uiRenderer->render(mainLayout,
                           ctx); // Render the main layout and its children

        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}