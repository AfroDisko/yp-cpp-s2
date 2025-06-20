#pragma once

#include <cstddef>
#include <algorithm>
#include <tuple>

namespace stdx::details {

template<std::size_t size_ = 1>
struct fixed_string {
    constexpr fixed_string(const char (&rhs)[size_]) {
        std::copy_n(rhs, size_, data);
    }

    template<std::size_t rhs_size>
    constexpr fixed_string(const char (&rhs)[rhs_size]) {
        static_assert(rhs_size <= size_, "string size exceeds buffer size");
        std::copy_n(rhs, rhs_size, data);
    }

    template<std::size_t rhs1_size, std::size_t rhs2_size>
    constexpr fixed_string(const char (&rhs1)[rhs1_size], const char (&rhs2)[rhs2_size]) {
        static_assert(rhs1_size + rhs2_size <= size_, "string sizes exceed buffer size");
        std::copy_n(rhs1, rhs1_size, data);
        std::copy_n(rhs2, rhs2_size, data + rhs1_size - 1);
    }

    constexpr std::size_t size() const { return size_; }

    constexpr char& operator[](std::size_t idx) {
        // static_assert(idx < size_, "index is out of bounds");
        return data[idx];
    }

    constexpr char operator[](std::size_t idx) const {
        // static_assert(idx < size_, "index is out of bounds");
        return data[idx];
    }

    constexpr operator const char*() const {
        return data;
    }

    char data[size_] = {};
};

struct parse_error : fixed_string<512> {};

template <typename... Ts>
struct scan_result {
    constexpr const auto& values() const { return tuple; }

    std::tuple<Ts...> tuple;
};

} // namespace stdx::details
