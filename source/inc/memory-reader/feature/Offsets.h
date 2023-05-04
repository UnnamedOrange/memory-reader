/**
 * @file Offsets.h
 * @author UnnamedOrange
 * @brief Structures storing offsets and type.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <cstdint>
#include <ranges>
#include <span>

#include "../process/IReadMemory.h"
#include "../utils/macro.h"

MEMORY_READER_NAMESPACE_BEGIN

namespace __detail {
    template <typename T>
    inline std::optional<T> offset_read_once(const IReadMemory& reader, std::uintptr_t base,
                                             std::intptr_t offset) noexcept {
        auto addr = base + static_cast<std::uintptr_t>(offset);
        return reader.read<T>(addr);
    }
    template <PtrWidth width>
    inline std::optional<std::uintptr_t> offset_read_ptr_once(const IReadMemory& reader, std::uintptr_t base,
                                                              std::intptr_t offset) noexcept {
        auto addr = base + static_cast<std::uintptr_t>(offset);
        // Invoke type check here.
        return reader.read<width>(addr);
    }
    template <PtrWidth width, typename T, std::intptr_t crt_offset, std::intptr_t... consequent_offsets>
    inline std::optional<T> template_offsets_read(const IReadMemory& reader, std::uintptr_t base) noexcept {
        if constexpr (sizeof...(consequent_offsets) == 0) {
            return offset_read_once<T>(reader, base, crt_offset);
        } else {
            auto addr = offset_read_ptr_once<width>(reader, base, crt_offset);
            if (!addr) {
                return std::nullopt;
            }
            return template_offsets_read<width, T, consequent_offsets...>(reader, *addr);
        }
    }
} // namespace __detail

template <PtrWidth width, typename T, std::intptr_t... offsets>
class ValueOffsets {
public:
    std::optional<T> read(const IReadMemory& reader, std::uintptr_t base) const noexcept {
        return __detail::template_offsets_read<width, T, offsets...>(reader, base);
    }
};

template <PtrWidth width, std::intptr_t... offsets>
using PtrOffsets = ValueOffsets<width, PtrType<width>, offsets...>;

MEMORY_READER_NAMESPACE_END
