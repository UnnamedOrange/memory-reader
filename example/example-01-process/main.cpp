/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief Example of @ref Process.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <cstdint>
#include <iostream>
#include <optional>
#include <thread>

#include <memory-reader/all.h>

using namespace orange::memory_reader;

void read_memory(const IReadMemory& reader) {
    int value = 114514;
    const int* pointer = &value;

    // Use read<T> to read plain old data.
    // A pointer in parameters should always be std::uintptr_t.
    auto value_read = reader.read<int>(reinterpret_cast<std::uintptr_t>(&value));
    // After calling read series, check the return value.
    if (!value_read) { // if (!value_read.has_value())
        return;
    }
    std::cout << (*value_read) << std::endl;
    // You may use exception to check empty std::optional.
    try {
        std::cout << value_read.value() << std::endl;
    } catch (const std::bad_optional_access&) {
        return;
    }

    // Use read<PtrWidth> to read a pointer.
    // Remember to specify the pointer width in the target process.
    // In real scenarios, you may use @ref PtrWidth::IS_32 or @ref PtrWidth::IS_64.
    auto pointer_read = reader.read<PtrWidth::IS_CURRENT>(reinterpret_cast<std::uintptr_t>(&pointer));
    if (!pointer_read) {
        return;
    }
    std::cout << reinterpret_cast<const void*>(*pointer_read) << std::endl;
}
void get_regions(const IReadMemory& reader) {
    // Use get_regions to get regions with execution perssion, for signature scanning.
    auto regions = reader.regions();
    // If failed, the return value is an empty vector.
    if (regions.empty()) {
        return;
    }
    // A @ref Region has two fields, base and size.
    for (const auto& region : regions) {
        std::cout << reinterpret_cast<const void*>(region.base) << ' ' << region.size << std::endl;
    }
}

void check_process(const AbstractProcess& p) {
    // If process is not alive, there should be no cache,
    // so get_cache_hint makes no sense.
    if (!p.still_alive()) {
        return;
    }
    // Use get_cache_hint to record the cache hint.
    auto cache_hint = p.get_cache_hint();
    // You may use cache hint to ensure the process is the same as before.
    if (p.still_alive() && cache_hint == p.get_cache_hint()) {
        std::cout << "The process is still alive." << std::endl;
    } else {
        std::cout << "Another process!" << std::endl;
    }
}

void wait_for_exit(IProcessSynchronize& sync) {
    std::thread t([&] {
        // You may wait the process to exit in another thread.
        sync.wait_until_exit();
    });
    // In the current thread, you should call interrupt_synchronize to interrupt wait_until_exit.
    // Note that @ref IProcessSynchronize should not be const.
    sync.interrupt_synchronize();
    t.join();
    std::cout << "Synchronization has been interrupted." << std::endl;
}

int main() {
    // Use static method try_from_* to access a process.
    // In real scenarios, you may use try_from_pid or try_from_process_name.
    auto p = Process::try_from_current_process();
    // After calling try_from_*, call empty to check whether it succeeded.
    if (p.empty())
        return 1;

    read_memory(p);
    get_regions(p);
    check_process(p);
    wait_for_exit(p);
}
