/**
 * @file TestProcess.cpp
 * @author UnnamedOrange
 * @brief Test @ref Process.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <array>
#include <chrono>
#include <cstring>
#include <thread>
#include <utility>

#include <gtest/gtest.h>

#include <memory-reader/all.h>

USING_MEMORY_READER_NAMESPACE;

TEST(TestProcess, test_empty) {
    Process p;
    ASSERT_TRUE(p.empty()) << "Should be empty after default construction.";
}
TEST(TestProcess, test_cache_hint) {
    Process p;
    // If Process is empty, cache hint makes no sense, so no test here.
    p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }
    ASSERT_NE(p.get_cache_hint(), 0) << "Cache hint should not be 0 after holding a process.";
}
TEST(TestProcess, test_cache_hint_movement_construct_and_assign) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    ASSERT_NE(p.get_cache_hint(), 0) << "Cache hint should not be 0 after holding a process.";

    Process another;
    int old_cache_hint = another.get_cache_hint();
    another = std::move(p);
    ASSERT_NE(old_cache_hint, another.get_cache_hint()) << "Cache hint should be different after a process moves in.";
}
TEST(TestProcess, test_still_alive) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }
    ASSERT_TRUE(p.still_alive()) << "Still alive should be true for current process.";
}
TEST(TestProcess, test_wait_and_interrupt) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    using namespace std::chrono;
    using namespace std::literals;

    auto start = steady_clock::now();
    std::thread t([&]() { p.wait_until_exit(); });
    std::this_thread::sleep_for(500ms);
    p.interrupt_synchronize();
    t.join();
    auto elapse = steady_clock::now() - start;
    ASSERT_LT(elapse, 533ms) << "Interrupting should be fast enough."
                             << " (elapse = " << (duration_cast<milliseconds>(elapse).count()) << "ms)";
}
TEST(TestProcess, test_regions) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    auto r = p.regions();
    ASSERT_FALSE(r.empty()) << "Regions should not be empty.";
}
TEST(TestProcess, test_read_pod) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    int ground_truth = 114514;
    auto read = p.read<int>(reinterpret_cast<std::uintptr_t>(&ground_truth));
    if (!read) {
        GTEST_SKIP() << "Cannot read from current process unexpectedly.";
    }
    ASSERT_EQ(*read, ground_truth) << "Read value should be the same as ground truth.";
}
TEST(TestProcess, test_read_ptr) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    int dummy{};
    int* ground_truth = &dummy;
    auto read = p.read<PtrWidth::IS_CURRENT>(reinterpret_cast<std::uintptr_t>(&ground_truth));
    if (!read) {
        GTEST_SKIP() << "Cannot read from current process unexpectedly.";
    }
    ASSERT_EQ(reinterpret_cast<const int*>(*read), &dummy) << "Read value should be the same as ground truth.";
}
TEST(TestProcess, test_read_bytes) {
    auto p = Process::try_from_current_process();
    if (p.empty()) {
        GTEST_SKIP() << "Cannot get current process unexpectedly.";
    }

    std::array<int, 3> ground_truth{114, 514, 1919810};
    auto read = p.read_bytes(reinterpret_cast<std::uintptr_t>(ground_truth.data()), sizeof(ground_truth));
    if (read.empty()) {
        GTEST_SKIP() << "Cannot read from current process unexpectedly.";
    }
    ASSERT_EQ(sizeof(ground_truth), read.size()) << "If read is successful, the size should be the same.";
    ASSERT_TRUE(std::memcmp(ground_truth.data(), read.data(), read.size()) == 0);
}
