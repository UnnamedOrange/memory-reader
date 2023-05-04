/**
 * @file AbstractProcess.h
 * @author UnnamedOrange
 * @brief Abstract process with interfaces defined and partially implemented.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>

#include "../utils/macro.h"
#include "IProcessAlive.h"
#include "IProcessSynchronize.h"
#include "IReadMemoryWithCacheHint.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Abstract process with interfaces defined and partially implemented.
 */
class AbstractProcess : public IProcessSynchronize, public IReadMemoryWithCacheHint {
    using Self = AbstractProcess;

private:
    bool has_interrupt = false;
    mutable std::mutex m_exit;
    mutable std::condition_variable cv_exit;

    inline static std::atomic<int> global_cache_hint;
    int cache_hint = 0;

public:
    AbstractProcess() noexcept = default;
    AbstractProcess(const Self& other) = delete;
    Self& operator=(const Self& other) = delete;
    AbstractProcess(Self&&) noexcept;
    Self& operator=(Self&&) noexcept;

    virtual ~AbstractProcess();

    // Implements IProcessSynchronize.
public:
    void wait_until_exit() const noexcept override;
    void interrupt_synchronize() noexcept override;

    // Implements IReadMemoryWithCacheHint.
public:
    [[nodiscard]] int get_cache_hint() const noexcept override;

public:
    /**
     * @brief Reset the process to empty state.
     */
    virtual void reset() noexcept = 0;
    /**
     * @brief Return whether the process is in empty state.
     *
     * @note This method is reentrant.
     */
    [[nodiscard]] virtual bool empty() const noexcept = 0;

protected:
    void update_cache_hint() noexcept;
};

MEMORY_READER_NAMESPACE_END
