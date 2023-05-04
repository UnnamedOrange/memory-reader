/**
 * @file Process_windows.cpp
 * @author UnnamedOrange
 * @brief Implement @ref AbstractProcess on Windows.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#ifdef MEMORY_READER_TARGET_PLATFORM_WIN32

#include "process/Process.h"

#include <utility>

#include <Windows.h>
#include <tlhelp32.h>

#include "utils/codecvt.h"
#include "utils/macro.h"

USING_MEMORY_READER_NAMESPACE;

using Self = Process;

struct Self::Impl {
    HANDLE handle{};
};

Self::Process() noexcept : pimpl{std::make_unique<Impl>()} {}
Self::Process(Self&& other) noexcept : pimpl{std::make_unique<Impl>()} {
    *this = std::move(other);
}
Self& Self::operator=(Self&& other) noexcept {
    Super::operator=(std::move(other));
    if (this != &other) {
        Self::reset();
        pimpl->handle = other.pimpl->handle;
        other.pimpl->handle = nullptr;
    }
    return *this;
}

Self::~Process() {
    Self::reset();
}

Self Self::try_from_current_process() noexcept {
    // We do not use GetCurrentProcess, so the handle is entitled to limited access.
    return try_from_pid(GetCurrentProcessId());
}
Self Self::try_from_pid(std::uint32_t pid) noexcept {
    HANDLE handle_try = OpenProcess(SYNCHRONIZE | PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_READ, false, pid);
    if (!handle_try)
        return {};
    Self ret;
    ret.pimpl->handle = handle_try;
    ret.update_cache_hint();
    return ret;
}
Self Self::try_from_process_name(const std::string& process_name) noexcept {
    class ProcessSnapshot final {
    private:
        HANDLE handle;

    public:
        ProcessSnapshot() noexcept {
            // If failed, handle will be INVALID_HANDLE_VALUE.
            handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        }
        ~ProcessSnapshot() noexcept {
            if (!empty())
                CloseHandle(handle);
        }

    public:
        bool empty() const noexcept {
            return handle == INVALID_HANDLE_VALUE;
        }
        operator HANDLE() const noexcept {
            return handle;
        }
    };

    std::wstring w_process_name = codecvt::to_wstring<char>(process_name);
    ProcessSnapshot snapshot;
    if (snapshot.empty())
        return {};
    PROCESSENTRY32W pe;
    pe.dwSize = sizeof(pe);
    if (!Process32FirstW(snapshot, &pe))
        return {};
    do {
        if (pe.szExeFile == w_process_name)
            return Self::try_from_pid(pe.th32ProcessID);
    } while (Process32NextW(snapshot, &pe));
    return {};
}

bool Self::empty() const noexcept {
    return !pimpl->handle;
}
void Self::reset() noexcept {
    if (auto& handle = pimpl->handle) {
        CloseHandle(handle);
        handle = nullptr;
    }
}

bool Self::read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept {
    SIZE_T read{};
    return ReadProcessMemory(pimpl->handle, reinterpret_cast<LPCVOID>(address), buf, size, &read) && read == size;
}
std::vector<Region> Self::regions() const noexcept {
    std::vector<Region> ret;

    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    auto min_address = sys_info.lpMinimumApplicationAddress;
    auto max_address = sys_info.lpMaximumApplicationAddress;

    auto crt_handle = pimpl->handle;
    auto crt_address = min_address;
    while (crt_address < max_address) {
        MEMORY_BASIC_INFORMATION mem_info;
        if (!VirtualQueryEx(crt_handle, crt_address, &mem_info, sizeof(mem_info)))
            return {};

        if ((mem_info.Protect & (PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE)) && //
            (mem_info.State == MEM_COMMIT)) {
            ret.emplace_back(Region{
                .base = reinterpret_cast<std::uintptr_t>(mem_info.BaseAddress),
                .size = static_cast<std::size_t>(mem_info.RegionSize),
            });
        }
        crt_address = (PVOID)((uintptr_t)(crt_address) + mem_info.RegionSize);
    }
    return ret;
}

bool Self::still_alive() const noexcept {
    if (empty())
        return false;
    DWORD exit_code{};
    GetExitCodeProcess(pimpl->handle, &exit_code); // Assume it always succeeds.
    return exit_code == STILL_ACTIVE;
}

#endif
