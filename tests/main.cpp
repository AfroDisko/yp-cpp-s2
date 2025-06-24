#include <cstdint>
#include <iostream>

#include <format_string.hpp>
#include <scan.hpp>

// #define CT_ERROR_TESTS

constexpr char test_string[] = "test string";
constexpr std::size_t test_size = sizeof(test_string);

int main(int argc, char* argv[]) {
{
    stdx::details::fixed_string str1(test_string);
    static_assert(str1.size() == test_size);
    std::cout << std::string(str1) << '\n';
}
{
    stdx::details::fixed_string<2 * test_size> str2(test_string, test_string);
    static_assert(str2.size() == 2 * test_size);
    std::cout << std::string(str2) << '\n';
}
{
    stdx::details::parse_error error("error");
    std::cout << std::string(error) << '\n';
}
{
    stdx::details::format_string<"format string"> fmt1;
    static_assert(fmt1.placeholders_count == 0);
    std::cout << std::string(fmt1.string) << '\n';
    std::cout << fmt1.placeholders_count << '\n';
}
{
    stdx::details::format_string<"format string {} with placeholders {%d}"> fmt2;
    static_assert(fmt2.placeholders_count == 2);
    std::cout << std::string(fmt2.string) << '\n';
    std::cout << fmt2.placeholders_count << '\n';
    std::cout << fmt2.placeholder_positions[0].first << " " << fmt2.placeholder_positions[0].second << '\n';
    std::cout << fmt2.placeholder_positions[1].first << " " << fmt2.placeholder_positions[1].second << '\n';
}
{
    using namespace stdx::literals;
    constexpr auto fmt_from_literal = "fmt from literal {} {}"_fs;
    std::cout << fmt_from_literal.string.data << '\n';
}
{
    constexpr stdx::details::format_string<"{%d} and {}"> fmt3;
    constexpr stdx::details::fixed_string source("42 and 3.1415926");
    constexpr auto bounds1 = stdx::details::get_current_source_for_parsing<0, fmt3, source>();
    static_assert(bounds1.first == 0);
    static_assert(bounds1.second == 2);
    std::cout << bounds1.first << " " << bounds1.second << '\n';
    constexpr auto bounds2 = stdx::details::get_current_source_for_parsing<1, fmt3, source>();
    static_assert(bounds2.first == 7);
    static_assert(bounds2.second == 16);
    std::cout << bounds2.first << " " << bounds2.second << '\n';
}
{
    constexpr auto sint = stdx::details::parse_value<std::int32_t>("-10");
    constexpr auto uint = stdx::details::parse_value<std::uint32_t>("10");
    constexpr auto str = stdx::details::parse_value<std::string_view>("some string");
    std::cout << sint << '\n';
    std::cout << uint << '\n';
    std::cout << str << '\n';
}
{
    constexpr stdx::details::format_string<"some text {%d}, txt {%u}, txt {%s}"> fmt;
    constexpr stdx::details::fixed_string src("some text -15, txt 10, txt string");
    std::cout << stdx::details::parse_input<0, fmt, src, std::int32_t>() << '\n';
    std::cout << stdx::details::parse_input<1, fmt, src, std::uint32_t>() << '\n';
    std::cout << stdx::details::parse_input<2, fmt, src, std::string_view>() << '\n';
}
{
    constexpr stdx::details::format_string<"{}, {}, {}"> fmt;
    constexpr stdx::details::fixed_string src("-30, 20, sstring");
    std::cout << stdx::details::parse_input<0, fmt, src, std::int32_t>() << '\n';
    std::cout << stdx::details::parse_input<1, fmt, src, std::uint32_t>() << '\n';
    std::cout << stdx::details::parse_input<2, fmt, src, std::string_view>() << '\n';
}

{
    constexpr stdx::details::format_string<"some str {%d} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str -45 some str");
    constexpr auto result = stdx::scan<fmt, src, std::int32_t>();
    static_assert(std::get<0>(result.values()) == -45);
    std::cout << std::get<0>(result.values()) << '\n';
}
{
    constexpr stdx::details::format_string<"some str {%u} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 30 some str");
    constexpr auto result = stdx::scan<fmt, src, std::uint32_t>();
    static_assert(std::get<0>(result.values()) == 30);
    std::cout << std::get<0>(result.values()) << '\n';
}
{
    constexpr stdx::details::format_string<"some str {%s} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str string some str");
    constexpr auto result = stdx::scan<fmt, src, std::string_view>();
    static_assert(std::get<0>(result.values()) == std::string_view("string"));
    std::cout << std::get<0>(result.values()) << '\n';
}
{
    constexpr stdx::details::format_string<"some str {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str string some str");
    constexpr auto result = stdx::scan<fmt, src, std::string_view>();
    static_assert(std::get<0>(result.values()) == std::string_view("string"));
    std::cout << std::get<0>(result.values()) << '\n';
}
{
    constexpr stdx::details::format_string<"some str {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 1024 some str");
    constexpr auto result = stdx::scan<fmt, src, std::int32_t>();
    static_assert(std::get<0>(result.values()) == 1024);
    std::cout << std::get<0>(result.values()) << '\n';
}

#ifdef CT_ERROR_TESTS
{
    constexpr stdx::details::format_string<"some str   {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 16 some str");
    constexpr auto result = stdx::scan<fmt, src, std::uint8_t>();
}
{
    constexpr stdx::details::format_string<"some str {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 1024 some str");
    constexpr auto result = stdx::scan<fmt, src, std::uint8_t>();
}
{
    constexpr stdx::details::format_string<"some str {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str abc some str");
    constexpr auto result = stdx::scan<fmt, src, std::uint8_t>();
}
{
    constexpr stdx::details::format_string<"some str {%u} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 1024 some str");
    constexpr auto result = stdx::scan<fmt, src, std::int32_t>();
}
{
    constexpr stdx::details::format_string<"some str {%d some str"> fmt;
    constexpr stdx::details::fixed_string src("some str 1024 some str");
    constexpr auto result = stdx::scan<fmt, src, std::int32_t>();
}
{
    constexpr stdx::details::format_string<"some str {} some str"> fmt;
    constexpr stdx::details::fixed_string src("some str some str");
    constexpr auto result = stdx::scan<fmt, src, std::int32_t>();
}
#endif
}
