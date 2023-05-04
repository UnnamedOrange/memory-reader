/**
 * @file IProcessAlive.h
 * @author UnnamedOrange
 * @brief Interface of checking whether a process is still alive.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "../utils/macro.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Interface of checking whether a process is still alive.
 */
struct IProcessAlive {
    /**
     * @brief Return whether the process is still alive.
     * If there is no process held, return false.
     *
     * @note This method should be reentrant.
     */
    [[nodiscard]] virtual bool still_alive() const noexcept = 0;
};

MEMORY_READER_NAMESPACE_END
