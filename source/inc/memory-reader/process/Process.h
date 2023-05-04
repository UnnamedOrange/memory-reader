/**
 * @file Process.h
 * @author UnnamedOrange
 * @brief Implementation of platform-dependent process.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "../utils/macro.h"
#include "AbstractProcess.h"

MEMORY_READER_NAMESPACE_BEGIN

/**
 * @brief Implementation of platform-dependent process.
 */
class Process : public AbstractProcess {
    using Self = Process;
    using Super = AbstractProcess;

private:
    struct Impl;
    std::unique_ptr<Impl> pimpl;

public:
    Process() noexcept;
    Process(Self&& other) noexcept;
    Self& operator=(Self&& other) noexcept;

    ~Process();

public:
    /**
     * @brief Try to create a @ref Process object holding the current process.
     *
     * @note This is mainly for test purpose. Should never fail.
     *
     * @return Process If succeeded, return a @ref Process object holding the current process.
     * If failed, return an empty @ref Process object.
     */
    [[nodiscard]] static Self try_from_current_process() noexcept;
    /**
     * @brief Try to create a @ref Process object holding the process with the given PID.
     *
     * @param pid ID of the process. Typically it is a uint32_t.
     * @return Process If succeeded, return a @ref Process object holding the process with the given PID.
     * If failed, return an empty @ref Process object.
     */
    [[nodiscard]] static Self try_from_pid(std::uint32_t pid) noexcept;
    /**
     * @brief Try to create a @ref Process object holding the process with the given name.
     * If there are multiple processes with the same name, the "first" one will be chosen.
     * The order is not guaranteed.
     *
     * @param process_name Name of the process.
     * @return Process If succeeded, return a @ref Process object holding the process with the given name.
     * If failed, return an empty @ref Process object.
     */
    [[nodiscard]] static Self try_from_process_name(const std::string& process_name) noexcept;

    // Implements AbstractProcess.
public:
    void reset() noexcept override;
    [[nodiscard]] bool empty() const noexcept override;

    // Implements IReadMemory.
public:
    [[nodiscard]] bool read_to_buf(std::uintptr_t address, void* buf, std::size_t size) const noexcept override;
    [[nodiscard]] std::vector<Region> regions() const noexcept override;

    // Implements IProcessAlive.
public:
    [[nodiscard]] bool still_alive() const noexcept override;
};

MEMORY_READER_NAMESPACE_END
