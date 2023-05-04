/**
 * @file TestPattern.cpp
 * @author UnnamedOrange
 * @brief Test @ref Pattern.
 *
 * @copyright Copyright (c) UnnamedOrange. Licensed under the MIT License.
 * See the LICENSE file in the repository root for full license text.
 */

#include <cstddef>

#include <gtest/gtest.h>

#include <memory-reader/all.h>

USING_MEMORY_READER_NAMESPACE;

TEST(TestPattern, test_constexpr_construction) {
    constexpr char t[] = "11 45 14 ??";
    auto p = Pattern(t);
    const auto cp = Pattern(t);
    constexpr auto cep = Pattern(t);
    ASSERT_EQ(p[0].byte, std::byte(0x11));
    ASSERT_EQ(p[0].is_mask, false);
    ASSERT_EQ(cp[1].byte, std::byte(0x45));
    ASSERT_EQ(cp[1].is_mask, false);
    ASSERT_EQ(cep[2].byte, std::byte(0x14));
    ASSERT_EQ(cep[2].is_mask, false);
    ASSERT_EQ(cep[3].is_mask, true);
}
TEST(TestPattern, test_dynamic_construction) {
    constexpr char t[] = "11 45 14 ??";
    auto p = DynamicPattern(t);
    const auto cp = DynamicPattern(t);
    ASSERT_EQ(p[0].byte, std::byte(0x11));
    ASSERT_EQ(p[0].is_mask, false);
    ASSERT_EQ(cp[1].byte, std::byte(0x45));
    ASSERT_EQ(cp[1].is_mask, false);
    ASSERT_EQ(cp[2].byte, std::byte(0x14));
    ASSERT_EQ(cp[2].is_mask, false);
    ASSERT_EQ(cp[3].is_mask, true);
}
TEST(TestPattern, test_dynamic_modification) {
    constexpr char t[] = "11 45 14 ??";
    auto p = DynamicPattern(t);
    p.emplace_back(PatternElement{.byte = std::byte(0x00), .is_mask = false});
    ASSERT_EQ(p.size(), 5);
    p = DynamicPattern(t);
    ASSERT_EQ(p.size(), 4);
}
