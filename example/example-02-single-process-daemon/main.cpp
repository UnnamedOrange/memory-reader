/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief Example of @ref SingleProcessDaemon.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <chrono>
#include <cstdint>
#include <iostream>
#include <thread>

#include <memory-reader/all.h>

using namespace orange::memory_reader;

int example_value = 1;

class Server {
private:
    // Change the process name as you want.
    SingleProcessDaemon process{"example-02-single-process-daemon"};

    int cache_hint{};

public:
    auto read_feature() {
        if (int new_cache_hint = process.get_cache_hint(); process.still_alive() && cache_hint != new_cache_hint) {
            std::cout << "Cache hint changed. Regard it as a new process." << std::endl;
            cache_hint = new_cache_hint;
        }
        return process.read<int>(reinterpret_cast<std::uintptr_t>(&example_value));
    }
};

int main() {
    Server server;
    // Shortly, the process can be opened by the daemon.
    // The duration in which process remains empty is not guaranteed,
    // and varies in different platforms.
    //
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

    for (int i = 0; i < 60; i++) {
        auto read_result = server.read_feature();
        if (!read_result) {
            std::cout << "Fail to read_feature." << std::endl;
        } else {
            std::cout << "read_feature: " << *read_result << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    // SingleProcessDaemon will automatically stop its inner thread on destruction.
}
