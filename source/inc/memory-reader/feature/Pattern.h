/**
 * @file Pattern.h
 * @author UnnamedOrange
 * @brief Structure storing signature pattern.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include "../utils/macro.h"

MEMORY_READER_NAMESPACE_BEGIN

inline constexpr auto DynamicPatternFlag = (std::numeric_limits<std::size_t>::max)();

struct PatternElement {
    std::byte byte;
    bool is_mask;
};

template <std::size_t RAW_SIZE>
    requires(RAW_SIZE != 0 && RAW_SIZE % 3 == 0)
class Pattern : public std::array<PatternElement, RAW_SIZE / 3> {
private:
    /**
     * @brief Convert a hexadecimal digit character to uint8_t.
     */
    static consteval std::uint8_t to_digit(char ch) {
        if ('0' <= ch && ch <= '9')
            return ch - '0';
        else if ('A' <= ch && ch <= 'F')
            return ch - 'A' + 10;
        else if ('a' <= ch && ch <= 'f')
            return ch - 'a' + 10;
        throw;
    }
    /**
     * @brief Concatenate two hexadecimal digits to one std::byte.
     */
    static consteval std::byte concat_digit(std::uint8_t low_digit, std::uint8_t high_digit) {
        if (!(low_digit > 0xF || high_digit > 0xF))
            return std::byte((high_digit << 4) | low_digit);
        throw;
    }

public:
    consteval Pattern(const char (&pattern_str)[RAW_SIZE]) noexcept {
        for (std::size_t i = 0; i < this->size(); i++) {
            char high_ch = pattern_str[i * 3 + 0];
            char low_ch = pattern_str[i * 3 + 1];
            if (low_ch == '?' || high_ch == '?') {
                (*this)[i] = {.byte = std::byte{}, .is_mask = true};
            } else {
                (*this)[i] = {.byte = concat_digit(to_digit(low_ch), to_digit(high_ch)), .is_mask = false};
            }
        }
    }
};

class DynamicPattern : public std::vector<PatternElement> {
    using Self = DynamicPattern;

public:
    DynamicPattern() noexcept = default;
    DynamicPattern(std::string_view pattern_str) {
        Self& self = *this;
        if ((pattern_str.length() + 1) % 3 != 0)
            throw std::invalid_argument("Length of pattern_str is invalid.");
        self.resize((pattern_str.length() + 1) / 3);
        for (std::size_t i = 0; i < self.size(); i++) {
            auto number = std::string(pattern_str.substr(i * 3, 2));
            if (number[0] == '?' || number[1] == '?') {
                self[i] = {.byte = std::byte{}, .is_mask = true};
            } else {
                self[i] = {.byte = static_cast<std::byte>(std::stoul(number, nullptr, 16)), .is_mask = false};
            }
        }
    }
};

MEMORY_READER_NAMESPACE_END
