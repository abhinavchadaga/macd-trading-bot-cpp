#pragma once

namespace async_rest_client {
namespace detail {

template<typename T>
struct always_false {
    static constexpr bool value = false;
};

template<typename T>
constexpr bool always_false_v = always_false<T>::value;

}
}