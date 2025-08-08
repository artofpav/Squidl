#pragma once
#include <Squidl/base/UIElement.h>
#include <Squidl/core/IRenderer.h>
#include <Squidl/core/UIContext.h>
#include <Squidl/elements/Label.h>
#include <Squidl/utils/Color.h>
#include <functional>
#include <memory>
#include <string>

namespace Squidl::Elements {

    class SQUIDL_API Checkbox : public Squidl::Base::UIElement {
      public:
        Checkbox(const std::string &labelText, int x, int y,
                 int size,  bool isChecked = false, TTF_Font *font = nullptr);

        bool update(Squidl::Core::UIContext &ctx,
                    Squidl::Core::IRenderer &renderer) override;
        void onEvent(Squidl::Core::UIEvent &event) override;
        void autosize() override;
        void updateBackdrop(Squidl::Core::UIContext &ctx,
                            Squidl::Core::IRenderer &renderer) override;

        bool isChecked() const { return mIsChecked; }
        void setChecked(bool checked) { mIsChecked = checked; }
        void setOnToggleCallback(std::function<void(bool)> callback) {
            mToggleCallback = callback;
        }

      private:
        bool mIsChecked;
        std::shared_ptr<Label> mLabel;
        std::function<void(bool)> mToggleCallback;

        Squidl::Utils::Color mCheckboxColor = {200, 200, 200, 255};
        Squidl::Utils::Color mCheckmarkColor = {0, 0, 0, 255};

        Squidl::Utils::UIRect calcBoxRect() const;
    };

} // namespace Squidl::Elements