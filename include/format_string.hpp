#pragma once

#include <cstddef>
#include <expected>
#include <array>

#include "types.hpp"

namespace stdx {

namespace details {

template<fixed_string string_>
struct format_string {
    static constexpr fixed_string string = string_;

    static consteval std::size_t size() { return string.size(); }
    static consteval std::expected<std::size_t, parse_error> get_placeholders_count();

    static constexpr auto placeholders_count_result = get_placeholders_count();
    static_assert(placeholders_count_result.has_value());
    static constexpr std::size_t placeholders_count = placeholders_count_result.value();

    using PlaceholderPosition = std::pair<std::size_t, std::size_t>;
    static consteval auto get_placeholder_positions();
    static constexpr auto placeholder_positions = get_placeholder_positions();
};

template<fixed_string string_>
consteval std::expected<std::size_t, parse_error> format_string<string_>::get_placeholders_count() {
    static constexpr auto is_valid_spec = [](char c) {
        static constexpr std::array valid_specs = {'d', 'u', 'f', 's'};
        for(char valid_spec : valid_specs) {
            if(c == valid_spec) {
                return true;
            }
        }
        return false;
    };

    constexpr std::size_t size = string.size();
    if (size == 0)
        return 0;

    std::size_t placeholder_count = 0;
    std::size_t position = 0;

    while (position < size) {
        if (string[position] != '{') {
            ++position;
            continue;
        }

        ++position;
        if (position >= size) {
            return std::unexpected(parse_error{"unclosed last placeholder"});
        }

        if (string[position] == '%') {
            ++position;
            if (position >= size) {
                return std::unexpected(parse_error{"unclosed last placeholder"});
            }
            if (!is_valid_spec(string[position])) {
                return std::unexpected(parse_error{"invalid specifier"});
            }
            ++position;
        }

        if (position >= size || string[position] != '}') {
            return std::unexpected(parse_error{"\'}\' is not found in appropriate place"});
        }
        ++position;

        ++placeholder_count;
    }

    return placeholder_count;
}

template<fixed_string string_>
consteval auto format_string<string_>::get_placeholder_positions() {
    std::array<PlaceholderPosition, placeholders_count> positions;

    std::size_t position = 0;
    std::size_t idx = 0;

    while(position < string.size()) {
        if(string[position] != '{') {
            ++position;
            continue;
        }
        const std::size_t shift = (string[position + 1] == '%') ? 3 : 1;
        positions[idx++] = {position, position + shift};
        position += shift + 1;
    }

    return positions;
}

}

namespace literals {

template<details::fixed_string str>
constexpr auto operator""_fs() {
    return details::format_string<str>{};
}

}

} // namespace stdx::details
