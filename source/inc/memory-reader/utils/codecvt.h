/**
 * @file codecvt.h
 * @author UnnamedOrange
 * @brief Provide utility functions for converting encodings of strings.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "macro.h"

MEMORY_READER_NAMESPACE_BEGIN

class codecvt final {
private:
    using path = std::filesystem::path;

public:
    template <typename T>
    [[nodiscard]] static std::string to_string(std::basic_string_view<T> str) {
        return path(str).string();
    }
    template <typename T>
    [[nodiscard]] static std::wstring to_wstring(std::basic_string_view<T> str) {
        return path(str).wstring();
    }
};

MEMORY_READER_NAMESPACE_END
