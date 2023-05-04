/**
 * @file Process_linux.cpp
 * @author UnnamedOrange
 * @brief Implement @ref AbstractProcess on Linux.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#ifdef MEMORY_READER_TARGET_PLATFORM_LINUX

#include "process/Process.h"

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils/macro.h"

USING_MEMORY_READER_NAMESPACE;

static std::uint64_t get_start_time(pid_t pid) noexcept {
    if (!pid)
        return 0;
    std::ifstream ifs("/proc/" + std::to_string(pid) + "/stat");
    if (ifs.fail())
        return 0;

    std::string buf;
    std::getline(ifs, buf);
    std::string_view sv(buf);
    for (size_t i = 0; i < 21; ++i) {
        auto pos = sv.find(' ');
        if (pos == std::string_view::npos)
            return 0;
        sv.remove_prefix(pos + 1);
    }
    sv.remove_suffix(sv.size() - sv.find(' '));

    std::uint64_t ret;
    try {
        ret = std::stoi(std::string(sv));
    } catch (...) {
        return 0;
    }
    return ret;
}

using Self = Process;

struct Self::Impl {
    /**
     * @brief On Linux, process ID is just enough.
     * Regard it as a handle.
     */
    pid_t pid{};
    /**
     * @brief The start time of the process.
     * Used to check whether the process is still alive.
     *
     * @see https://stackoverflow.com/a/62882645
     */
    std::uint64_t start_time{};
};

Self::Process() noexcept : pimpl{std::make_unique<Impl>()} {}
Self::Process(Self&& other) noexcept : pimpl{std::make_unique<Impl>()} {
    *this = std::move(other);
}
Self& Self::operator=(Self&& other) noexcept {
    Super::operator=(std::move(other));
    if (this != &other) {
        Self::reset();

        pimpl->pid = other.pimpl->pid;
        pimpl->start_time = other.pimpl->start_time;

        other.pimpl->pid = 0;
        other.pimpl->start_time = 0;
    }
    return *this;
}

Self::~Process() {
    Self::reset();
}

Self Self::try_from_current_process() noexcept {
    return try_from_pid(getpid());
}
Self Self::try_from_pid(std::uint32_t pid) noexcept {
    Self ret;
    ret.pimpl->pid = pid;
    ret.pimpl->start_time = get_start_time(pid);
    ret.update_cache_hint();
    return ret;
}
Self Self::try_from_process_name(const std::string& process_name) noexcept {
    namespace fs = std::filesystem;

    // proc/{}/comm truncates the name to 15 chars.
    // https://stackoverflow.com/questions/15545341/process-name-from-its-pid-in-linux#comment131417360_15545536
    constexpr auto process_name_from_pid = [](int pid) noexcept -> std::optional<std::string> {
        fs::path path;
        try {
            path = fs::canonical("/proc/" + std::to_string(pid) + "/exe");
        } catch (...) {
            return {};
        }
        return path.filename().string();
    };

    for (const auto& entry : fs::directory_iterator("/proc")) {
        if (!entry.is_directory())
            continue;

        int pid;
        try {
            pid = std::stoi(entry.path().filename());
        } catch (...) {
            continue;
        }

        auto crt_name = process_name_from_pid(pid);
        if (!crt_name)
            continue;
        if (crt_name == process_name)
            return try_from_pid(pid);
    }
    return {};
}

bool Self::empty() const noexcept {
    return !pimpl->pid;
}
void Self::reset() noexcept {
    pimpl->pid = 0;
    pimpl->start_time = 0;
}

bool Self::read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept {
    iovec local{
        .iov_base = buf,
        .iov_len = size,
    };
    iovec remote{
        .iov_base = reinterpret_cast<void*>(address),
        .iov_len = size,
    };
    // Assume process_vm_readv has been implemented in the kernel.
    auto result = process_vm_readv(pimpl->pid, &local, 1, &remote, 1, 0);
    if (result != -1)
        return static_cast<std::size_t>(result) == size;
    return false;
}
std::vector<Region> Self::regions() const noexcept {
    std::vector<Region> ret;

    std::ifstream ifs("/proc/" + std::to_string(pimpl->pid) + "/maps");
    if (ifs.fail())
        return {};

    std::string buf;
    while (std::getline(ifs, buf)) {
        std::uintptr_t start, end;
        std::array<char, 5> perms;
        auto result = std::sscanf(buf.c_str(), "%lx-%lx%4s", &start, &end, perms.data());
        if (result != 3)
            continue;
        if (perms[0] == 'r' && perms[2] == 'x') {
            ret.emplace_back(Region{
                .base = start,
                .size = static_cast<size_t>(end - start),
            });
        }
    }
    return ret;
}

bool Self::still_alive() const noexcept {
    if (empty())
        return false;
    namespace fs = std::filesystem;
    auto pid = pimpl->pid;
    if (!fs::exists("/proc/" + std::to_string(pid)))
        return false;
    if (get_start_time(pid) != pimpl->start_time)
        return false;
    return true;
}

#endif
