/**
 * @file SingleProcessDaemon.h
 * @author UnnamedOrange
 * @brief Provide a proxy for @ref Process class, polling to open by a certain name.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>

#include "../utils/macro.h"
#include "AbstractProcess.h"
#include "IProcessAlive.h"
#include "IReadMemoryWithCacheHint.h"
#include "Process.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Provide a proxy for @ref Process class, polling to open by a certain name.
 */
class SingleProcessDaemon final : public IProcessAlive, public IReadMemoryWithCacheHint {
    using Self = SingleProcessDaemon;

private:
    mutable std::mutex m_state;
    std::string desired_name;
    Process process;

    std::thread polling_thread{&Self::polling_thread_routine, this};
    bool should_exit = false;
    mutable std::mutex m_exit;
    mutable std::condition_variable cv_exit;

public:
    SingleProcessDaemon() noexcept = default;
    SingleProcessDaemon(const std::string& desired_name) noexcept;
    SingleProcessDaemon(const Self&) = delete;
    Self& operator=(const Self&) = delete;
    SingleProcessDaemon(Self&&) = delete;
    Self& operator=(Self&&) = delete;

    ~SingleProcessDaemon();

private:
    void polling_thread_routine();
    void interrupt();

    // Implements IProcessAlive.
public:
    [[nodiscard]] bool still_alive() const noexcept override;

    // Implements IReadMemory.
public:
    [[nodiscard]] bool read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept override;
    [[nodiscard]] std::vector<Region> regions() const noexcept override;

    // Implements IReadMemoryWithCacheHint.
public:
    [[nodiscard]] int get_cache_hint() const noexcept override;

public:
    /**
     * @brief Set the desired process name.
     *
     * @note This method is reentrant.
     */
    void set_process_name(const std::string& process_name);
};

MEMORY_READER_NAMESPACE_END
