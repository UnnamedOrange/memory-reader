/**
 * @file Signature.h
 * @author UnnamedOrange
 * @brief Signature scanning and caching.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <cstdint>
#include <mutex>
#include <optional>

#include "../process/IReadMemoryWithCacheHint.h"
#include "../utils/macro.h"
#include "Pattern.h"

MEMORY_READER_NAMESPACE_BEGIN

namespace __detail {
    /**
     * @note This method is reentrant, if @b pattern does not change during the procedure.
     */
    template <typename pattern_t>
    std::optional<std::uintptr_t> scan_impl(const IReadMemoryWithCacheHint& reader, const pattern_t& pattern) noexcept {
        auto regions = reader.regions();
        for (const auto& region : regions) {
            auto region_data = reader.read_bytes(region.base, region.size);
            if (region_data.empty())
                continue;

            for (std::size_t i = 0; i + pattern.size() - 1 < region_data.size(); i++) {
                bool ok = true;
                for (std::size_t j = 0; j < pattern.size(); j++)
                    if (!(pattern[j].is_mask || region_data[i + j] == pattern[j].byte)) {
                        ok = false;
                        break;
                    }
                if (ok)
                    return region.base + i;
            }
        }
        return std::nullopt;
    }
} // namespace __detail

/**
 * @brief Signature scanning and caching.
 */
template <Pattern pattern>
class Signature {
    using Self = Signature;

private:
    std::optional<int> cache_hint;
    std::optional<std::uintptr_t> cache;
    mutable std::mutex m_cache;

public:
    /**
     * @brief Scan the pattern using provided reader.
     * According to the cache hint, the cache may be returned directly.
     *
     * If there are multiple conforming patterns, any one of them will be returned.
     *
     * @note This method is reentrant.
     *
     * @return std::optional<std::uintptr_t> The address of the first byte of the pattern.
     * If any error occurs, return std::nullopt.
     */
    std::optional<std::uintptr_t> scan(const IReadMemoryWithCacheHint& reader) noexcept {
        if (std::lock_guard _lock(m_cache); true) {
            // reader.get_cache_hint() is reentrant.
            // Assume reader.get_cache_hint() does not change during this method.
            auto incoming_cache_hint = reader.get_cache_hint();
            // Only drop the cache when the cache hint is changed.
            // For other unexpected situations, just let failure happen in subsequent operations.
            if (cache_hint && incoming_cache_hint == *cache_hint) {
                return cache;
            } else {
                cache = __detail::scan_impl(reader, pattern);
                // Clear the cache hint on failure.
                if (cache) {
                    cache_hint = incoming_cache_hint;
                } else {
                    cache_hint = std::nullopt;
                }
            }
            return cache;
        }
    }
};

class DynamicSignature {
    using Self = DynamicSignature;

private:
    DynamicPattern pattern;
    std::optional<int> cache_hint;
    std::optional<std::uintptr_t> cache;
    mutable std::mutex m_cache;

public:
    DynamicSignature() noexcept = default;
    DynamicSignature(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    DynamicSignature(Self&&) = delete;
    Self& operator=(Self&&) = delete;

    DynamicSignature(const DynamicPattern& pattern) noexcept : pattern(pattern) {}

public:
    /**
     * @brief Scan the pattern using provided reader.
     * According to the cache hint, the cache may be returned directly.
     *
     * If there are multiple conforming patterns, any one of them will be returned.
     *
     * If the pattern is empty, return std::nullopt.
     *
     * @note This method is reentrant.
     *
     * @return std::optional<std::uintptr_t> The address of the first byte of the pattern.
     * If any error occurs, return std::nullopt.
     */
    std::optional<std::uintptr_t> scan(const IReadMemoryWithCacheHint& reader) noexcept {
        if (pattern.empty()) {
            return std::nullopt;
        }
        if (std::lock_guard _lock(m_cache); true) {
            // reader.get_cache_hint() is reentrant.
            // Assume reader.get_cache_hint() does not change during this method.
            auto incoming_cache_hint = reader.get_cache_hint();
            // Only drop the cache when the cache hint is changed.
            // For other unexpected situations, just let failure happen in subsequent operations.
            if (cache_hint && incoming_cache_hint == *cache_hint) {
                return cache;
            } else {
                cache = __detail::scan_impl(reader, pattern);
                // Clear the cache hint on failure.
                if (cache) {
                    cache_hint = incoming_cache_hint;
                } else {
                    cache_hint = std::nullopt;
                }
            }
            return cache;
        }
    }
};

MEMORY_READER_NAMESPACE_END
