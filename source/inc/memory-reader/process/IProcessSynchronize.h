/**
 * @file IProcessSynchronize.h
 * @author UnnamedOrange
 * @brief Interface of process synchronization.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include "../utils/macro.h"
#include "IProcessAlive.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Interface of process synchronization.
 */
struct IProcessSynchronize : IProcessAlive {
    /**
     * @brief Wait until the process exits.
     * If the process object is empty, return immediately.
     *
     * @note This method should be reentrant.
     *
     * @note Object that may call this method MUST have a shorter life span than this interface.
     */
    virtual void wait_until_exit() const noexcept = 0;
    /**
     * @brief Interrupt all @ref wait_until_exit.
     * Call this method in destructor of the subclass.
     */
    virtual void interrupt_synchronize() noexcept = 0;
};

MEMORY_READER_NAMESPACE_END
