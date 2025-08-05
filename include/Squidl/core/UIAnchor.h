#pragma once
#include "Squidl/SquidlConfig.h"
#include <algorithm>
namespace Squidl::Core {
    enum SQUIDL_API class UIAnchor : uint8_t {
        None = 0,
        Left = 1 << 0,
        Right = 1 << 1,
        Top = 1 << 2,
        Bottom = 1 << 3,
        HCenter = 1 << 4,
        VCenter = 1 << 5
    };

    inline UIAnchor operator|(UIAnchor a, UIAnchor b) {
        return static_cast<UIAnchor>(static_cast<uint8_t>(a) |
                                     static_cast<uint8_t>(b));
    }

    inline UIAnchor operator&(UIAnchor a, UIAnchor b) {
        return static_cast<UIAnchor>(static_cast<uint8_t>(a) &
                                     static_cast<uint8_t>(b));
    }

    inline UIAnchor &operator|=(UIAnchor &a, UIAnchor b) {
        a = a | b;
        return a;
    }

    inline UIAnchor &operator&=(UIAnchor &a, UIAnchor b) {
        a = a & b;
        return a;
    }

    inline bool hasFlag(UIAnchor value, UIAnchor flag) {
        return static_cast<uint8_t>(value & flag) != 0;
    }

} // namespace Squidl::Core