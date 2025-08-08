#pragma once

#include "Squidl/SquidlConfig.h"

namespace Squidl::Core {

    SQUIDL_API class Padding {

      public:
        int top;
        int left;
        int bottom;
        int right;

        Padding(){};
        Padding(int value) {
            top = value;
            left = value;
            bottom = value;
            right = value;
        }

        inline Padding operator=(int value) {
            top = value;
            left = value;
            bottom = value;
            right = value;
            return *this;
        };

        int pHeightSum(){return top + bottom;}
        int pWidthSum() { return left + right; }
    };

    enum SQUIDL_API class HorizontalAlign {
        Left,
        Center,
        Right,
        Stretch,
        Justify
    };

    enum SQUIDL_API class VerticalAlign {
        Top,
        Center,
        Bottom,
        Stretch,
        Justify
    };
} // namespace Squidl::Core
