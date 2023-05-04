/**
 * @file SingleProcessDaemon.cpp
 * @author UnnamedOrange
 * @brief Provide a proxy for @ref Process class, polling to open by a certain name.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include "process/SingleProcessDaemon.h"

#include <chrono>

#include "utils/macro.h"

USING_MEMORY_READER_NAMESPACE;

using Self = SingleProcessDaemon;

Self::SingleProcessDaemon(const std::string& desired_name) noexcept : desired_name(desired_name) {}

Self::~SingleProcessDaemon() {
    process.interrupt_synchronize();
    this->interrupt();
    polling_thread.join();
}

void Self::polling_thread_routine() {
    const auto try_opening_and_wait = [&] {
        if (std::lock_guard _(m_state); true) {
            if (desired_name.empty()) {
                return;
            }
            // Process is assumed empty before calling this method.
            process = Process::try_from_process_name(desired_name);
        }
        // If process is empty, wait_until_exit will return immediately.
        process.wait_until_exit();
        // Process is assumed exited (or empty) after calling this method.
    };

    using namespace std::literals;
    while (true) {
        try_opening_and_wait();
        // Sleep after each try.
        if (std::unique_lock lock(m_exit); true) {
            bool cv_result = cv_exit.wait_for(lock, 100ms, [&]() { return should_exit; });
            if (cv_result)
                break;
        }
    }
}
void Self::interrupt() {
    if (std::lock_guard _(m_exit); true) {
        should_exit = true;
    }
    cv_exit.notify_all();
}

bool Self::still_alive() const noexcept {
    return process.still_alive();
}

bool Self::read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept {
    return process.read_to_buf(address, buf, size);
}
std::vector<Region> Self::regions() const noexcept {
    return process.regions();
}

int Self::get_cache_hint() const noexcept {
    return process.get_cache_hint();
}

void Self::set_process_name(const std::string& process_name) {
    if (std::lock_guard _(m_state); true) {
        desired_name = process_name;
    }
}
