/**
 * @file main.cpp
 * @author UnnamedOrange
 * @brief Example of @ref ValueOffsets.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <cstdint>
#include <iostream>
#include <optional>

#include <memory-reader/all.h>

using namespace orange::memory_reader;

// Shorthands of PtrOffsets and ValueOffsets for 32-bit process.
template <std::intptr_t... R>
using POffsets = PtrOffsets<PtrWidth::IS_32, R...>;
template <typename T, std::intptr_t... R>
using VOffsets = ValueOffsets<PtrWidth::IS_32, T, R...>;

struct Hub {
    SingleProcessDaemon process{"osu!.exe"};

    Signature<"7D 15 A1 ?? ?? ?? ?? 85 C0"> //
        sig_rulesets;

    // (uintptr_t)[[base - 0xB] + 0x4]
    POffsets<-0xB, 0x4> //
        offsets_ruleset;

    // (uint16_t)[[[base + 0x68] + 0x38] + 0x94]
    VOffsets<std::uint16_t, 0x68, 0x38, 0x94> //
        offsets_combo;

    // VOffsets can be seen as POffsets<std::uintptr_t, ...>.
};

class Server {
private:
    Hub hub;

public:
    std::optional<std::uint16_t> get_combo() {
        auto base = hub.sig_rulesets.scan(hub.process);
        if (!base) {
            return std::nullopt;
        }
        auto ruleset = hub.offsets_ruleset.read(hub.process, *base);
        if (!ruleset) {
            return std::nullopt;
        }
        auto combo = hub.offsets_combo.read(hub.process, *ruleset);
        return combo;
    }
};

int main() {
    Server server;
    for (int i = 0; i < 300; i++) {
        auto combo = server.get_combo();
        if (combo) {
            std::cout << *combo << std::endl;
        } else {
            std::cout << "Fail to read combo." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
}
