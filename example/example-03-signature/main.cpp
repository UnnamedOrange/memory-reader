/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief Example of @ref Signature.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <chrono>
#include <iostream>
#include <memory>
#include <optional>
#include <thread>

#include <memory-reader/all.h>

using namespace orange::memory_reader;

struct Hub {
    SingleProcessDaemon process{"osu!.exe"};

    // Static signature leverages template to avoid any dynamic allocation.
    // The string literal is converted to Pattern implicitly.
    Signature<"7D 15 A1 ?? ?? ?? ?? 85 C0"> sig_rulesets;
    // For convenience in multi-threading scenarios, dynamic signature should
    // be wrapped in a shared_ptr.
    // To construct a dynamic signature, you need to pass a DynamicPattern.
    std::shared_ptr<DynamicSignature> dynamic_sig_rulesets =
        std::make_shared<DynamicSignature>(DynamicPattern("7D 15 A1 ?? ?? ?? ?? 85 C0"));
};

class Server {
private:
    Hub hub;

public:
    // Methods calling scan should be const.
    // You should aware Signature::scan() changes the cache.
    std::optional<std::uintptr_t> scan_base() noexcept {
        return hub.sig_rulesets.scan(hub.process);
    }

    std::optional<std::uintptr_t> dynamic_scan_base() noexcept {
        return hub.dynamic_sig_rulesets->scan(hub.process);
    }
};

int main() {
    Server server;
    for (std::size_t i = 0; i < 60; i++) {
        {
            auto base = server.scan_base();
            if (base) {
                std::cout << "Found base = " << std::hex << *base << std::endl;
            } else {
                std::cout << "Not found." << std::endl;
            }
        }
        {
            auto base = server.dynamic_scan_base();
            if (base) {
                std::cout << "Dynamic found base = " << std::hex << *base << std::endl;
            } else {
                std::cout << "Dynamic not found." << std::endl;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
