/**
 * @file IReadMemory.h
 * @author UnnamedOrange
 * @brief Interface of basic memory reading functions.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>
#include <type_traits>
#include <vector>

#include "../utils/macro.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Enum type used in @ref IReadMemory::read.
 */
enum class PtrWidth : std::size_t {
    /**
     * @brief The width of pointer in the to-be-read process is 32-bit.
     */
    IS_32 = 4,
    /**
     * @brief The width of pointer in the to-be-read process is 64-bit.
     */
    IS_64 = 8,
    /**
     * @brief The width of pointer in the to-be-read process is the same as the current process.
     *
     * @note This enum value should not be used unless the to-be-read process
     * is created by @ref Process::try_from_current_process.
     */
    IS_CURRENT = sizeof(std::uintptr_t),
};
/**
 * @brief Pointer type according to @ref PtrWidth.
 */
template <PtrWidth width>
using PtrType = std::conditional_t<width == PtrWidth::IS_32, std::uint32_t, std::uint64_t>;

struct Region {
    std::uintptr_t base;
    std::size_t size;
};

/**
 * @brief Interface of basic memory reading functions.
 */
struct IReadMemory {
    /**
     * @brief Read memory to a buffer.
     *
     * @note This method should be reentrant.
     *
     * @param address Starting address in the to-be-read process.
     * @param buf The buffer to hold the reading result.
     * @param size The number of bytes to be read. @b buf should be large enough to hold this number of bytes.
     * @return true Succeeded to read all.
     * @return false Failed to read all.
     * In this case @b buf may be polluted and the content inside should be discarded.
     */
    [[nodiscard]] virtual bool read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept = 0;
    /**
     * @brief Get all regions with execution permission.
     *
     * @note This method should be reentrant.
     */
    [[nodiscard]] virtual std::vector<Region> regions() const noexcept = 0;

    /**
     * @brief Read memory and return a plain old data type.
     *
     * @note This method is reentrant.
     *
     * @tparam T The type of the variable to be read.
     * @param address Starting address in the to-be-read process.
     * @return std::optional<T> If succeeded, return the value. Otherwise, return std::nullopt.
     */
    template <typename T>
        requires std::is_trivial_v<T> && std::is_standard_layout_v<T>
    std::optional<T> read(std::uintptr_t address) const noexcept {
        T buf;
        if (!read_to_buf(address, &buf, sizeof(T))) {
            return std::nullopt;
        }
        return buf;
    }
    /**
     * @brief Read a pointer in the to-be-read process.
     *
     * @note This method is reentrant.
     *
     * @see PtrWidth
     *
     * @tparam width The width of the pointer to be read.
     * @param address Starting address in the to-be-read process.
     * @return std::optional<std::uintptr_t> If succeeded, return the value. Otherwise, return std::nullopt.
     */
    template <PtrWidth width>
    std::optional<std::uintptr_t> read(std::uintptr_t address) const noexcept {
        static_assert(sizeof(uintptr_t) >= static_cast<std::size_t>(width), //
                      "Width is not long enough on current platform.");
        // PtrType<width> can be broadened as std::uintptr_t without warning.
        return read<PtrType<width>>(address);
    }
    /**
     * @brief Read memory and return a byte array.
     *
     * @note This method is reentrant.
     *
     * @param address Starting address in the to-be-read process.
     * @param size The number of bytes to be read.
     * @return std::vector<std::byte> If succeeded, return the array. Otherwise, return an empty array.
     */
    std::vector<std::byte> read_bytes(std::uintptr_t address, std::size_t size) const noexcept {
        std::vector<std::byte> buf(size);
        if (!read_to_buf(address, buf.data(), buf.size())) {
            return {};
        }
        return buf;
    }
};

MEMORY_READER_NAMESPACE_END
