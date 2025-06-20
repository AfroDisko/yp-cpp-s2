#pragma once

#include <cstdint>
#include <cstdlib>
#include <string_view>

namespace stdx {

namespace concepts {

template<typename T, typename... U>
concept is_any_of = (std::same_as<T, U> || ...);

template<typename T>
concept is_supported_int = is_any_of<T, std::int8_t, std::int16_t, std::int32_t, std::int64_t>;

template<typename T>
concept is_supported_uint = is_any_of<T, std::uint8_t, std::uint16_t, std::uint32_t, std::uint64_t>;

template<typename T>
concept is_supported_float = is_any_of<T, float, double, long double>;

template<typename T>
concept is_supported_str = is_any_of<T, std::string_view>;

template<typename T>
concept is_supported_type = is_supported_int<T> || is_supported_uint<T> || is_supported_str<T>;

} // namespace stdx::concepts

namespace details {

template<std::size_t I, format_string fmt, fixed_string source>
consteval auto get_current_source_for_parsing() {
    static_assert(I < fmt.placeholders_count, "invalid placeholder index");

    constexpr auto to_sv = [](const auto& fs) {
        return std::string_view(fs.data, fs.size() - 1);
    };

    constexpr auto fmt_sv = to_sv(fmt.string);
    constexpr auto src_sv = to_sv(source);
    const auto& positions = fmt.placeholder_positions;

    constexpr auto pos_i = positions[I];
    constexpr std::size_t fmt_start = pos_i.first;
    constexpr std::size_t fmt_end = pos_i.second;

    constexpr auto src_start = [&] {
        if constexpr (I == 0) {
            return fmt_start;
        } else {
            constexpr auto prev_bounds = get_current_source_for_parsing<I - 1, fmt, source>();
            constexpr auto prev_end = prev_bounds.second;

            constexpr auto prev_fmt_end = positions[I - 1].second;
            constexpr auto sep = fmt_sv.substr(prev_fmt_end + 1, fmt_start - (prev_fmt_end + 1));

            constexpr auto pos = src_sv.find(sep, prev_end);
            return (pos != std::string_view::npos) ? pos + sep.size() : src_sv.size();
        }
    }();

    constexpr auto src_end = [&]{
        if constexpr(fmt_end == (fmt_sv.size() - 1)) {
            return src_sv.size();
        }

        constexpr auto distance = (I < fmt.placeholders_count - 1) ? positions[I + 1].first - (fmt_end + 1) : fmt_sv.size() - (fmt_end + 1);
        constexpr auto sep = fmt_sv.substr(fmt_end + 1, distance);

        constexpr auto pos = src_sv.find(sep, src_start);
        return (pos != std::string_view::npos) ? pos : src_sv.size();
    }();

    return std::pair{src_start, src_end};
}

template<concepts::is_supported_type ReturnT>
consteval ReturnT parse_value(std::string_view str) {
    if constexpr (concepts::is_supported_str<ReturnT>) {
        return str;
    } else {
        ReturnT result = {};
        const auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

        if (ec == std::errc::invalid_argument) {
            throw "cannot recognize string pattern as number";
        }
        if (ec == std::errc::result_out_of_range) {
            throw "requested type is overflown";
        }

        return result;
    }
}

template<std::size_t I, format_string fmt, fixed_string source, typename ReturnT>
consteval ReturnT parse_input() {
    static_assert(I < fmt.placeholders_count, "invalid placeholder index");

    constexpr auto fmt_bounds = fmt.placeholder_positions[I]; 
    constexpr auto src_bounds = get_current_source_for_parsing<I, fmt, source>();

    static constexpr auto has_specifier = [](const auto& bounds) {
        return bounds.second - bounds.first > 1;
    };
    static constexpr auto get_specifier = [](const auto& bounds) {
        return fmt.string[bounds.first + 2];
    };
    static constexpr auto src_size = [](const auto& bounds) {
        return bounds.second - bounds.first;
    };
    static constexpr auto get_string_view = [](const auto& bounds) {
        return std::string_view(source.data + bounds.first, src_size(bounds));
    };

    if constexpr (has_specifier(fmt_bounds)) {
        constexpr auto specifier = get_specifier(fmt_bounds);
        if constexpr (specifier == 'd') {
            static_assert(concepts::is_supported_int<ReturnT>, "specifier 'd' corresponds to int types");
        } else if constexpr (specifier == 'u') {
            static_assert(concepts::is_supported_uint<ReturnT>, "specifier 'u' corresponds to uint types");
        } else if constexpr (specifier == 'f') {
            static_assert(concepts::is_supported_float<ReturnT>, "specifier 'f' corresponds to float types");
        } else if constexpr (specifier == 's') {
            static_assert(concepts::is_supported_str<ReturnT>, "specifier 's' corresponds to string types");
        } else {
            static_assert(false, "unknown specifier");
        }
    }
    return parse_value<ReturnT>(get_string_view(src_bounds));
}

} // namespace stdx::details

} // namespace stdx
