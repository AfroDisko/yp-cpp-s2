#pragma once

#include "parse.hpp"

namespace stdx {

template <details::format_string fmt, details::fixed_string source, typename... Ts>
consteval details::scan_result<Ts...> scan() {
    static_assert(fmt.placeholders_count == sizeof...(Ts), "placeholders count does not equal types count");

    static constexpr auto make_tuple = []<std::size_t... Is>(std::index_sequence<Is...>) {
        return std::make_tuple(details::parse_input<Is, fmt, source, typename std::tuple_element<Is, std::tuple<Ts...>>::type>()...);
    };

    return details::scan_result<Ts...>{make_tuple(std::make_index_sequence<sizeof...(Ts)>{})};
}

} // namespace stdx
