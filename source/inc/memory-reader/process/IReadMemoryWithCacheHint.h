/**
 * @file IReadMemoryWithCacheHint.h
 * @author UnnamedOrange
 * @brief Interface of basic memory reading functions and cache hint getter.
 * Cache hint can be used to tell the cache should be dropped.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "../utils/macro.h"
#include "IReadMemory.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Interface of basic memory reading functions and cache hint getter.
 * Cache hint can be used to tell the cache should be dropped.
 */
struct IReadMemoryWithCacheHint : public IReadMemory {
    /**
     * @brief Get the cache hint. Cache hint can be used to tell the cache should be dropped.
     *
     * Once the return value is different from the one you stored before, your cache MUST be dropped.
     * Otherwise, it is up to you to decide whether to drop the cache.
     *
     * @note This method should be reentrant.
     */
    [[nodiscard]] virtual int get_cache_hint() const noexcept = 0;
};

MEMORY_READER_NAMESPACE_END
