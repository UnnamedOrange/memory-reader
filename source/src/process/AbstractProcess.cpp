/**
 * @file AbstractProcess.cpp
 * @author UnnamedOrange
 * @brief Abstract process with interfaces defined and partially implemented.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include "process/AbstractProcess.h"

#include <chrono>

#include "utils/macro.h"

USING_MEMORY_READER_NAMESPACE;

using Self = AbstractProcess;

Self::AbstractProcess(Self&& other) noexcept {
    *this = std::move(other);
}
Self& Self::operator=(Self&& other) noexcept {
    // mutex and condition_variable cannot be moved, just default construct.

    this->cache_hint = other.cache_hint;
    other.cache_hint = 0;

    this->has_interrupt = other.has_interrupt;
    other.interrupt_synchronize();

    return *this;
}

Self::~AbstractProcess() {
    Self::interrupt_synchronize();
}

void Self::wait_until_exit() const noexcept {
    using namespace std::literals;
    while (still_alive()) {
        if (std::unique_lock lock(m_exit); true) {
            bool cv_result = cv_exit.wait_for(lock, 100ms, [&]() { return has_interrupt; });
            if (cv_result)
                break;
        }
    }
}
void Self::interrupt_synchronize() noexcept {
    if (std::lock_guard _lock(m_exit); true) {
        has_interrupt = true;
    }
    cv_exit.notify_all();
}

int Self::get_cache_hint() const noexcept {
    return cache_hint;
}

void Self::update_cache_hint() noexcept {
    cache_hint = ++global_cache_hint;
}
