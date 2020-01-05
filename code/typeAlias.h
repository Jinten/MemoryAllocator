#pragma once

using u8 = std::uint8_t;
using s32 = std::int32_t;
using u32 = std::uint32_t;

using Action = std::function<void()>;
using ArgAction = std::function<void(void*)>;

template<class T>
using TArgAction = std::function<void(T)>;