#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(RegexFlags, CaseInsensitiveMatching) {
    const boost::regex pattern("hello", boost::regex::icase);

    EXPECT_TRUE(boost::regex_match(std::string("HeLLo"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("world"), pattern));
}

TEST(RegexFlags, NoSubsSuppressesCaptureStorage) {
    const boost::regex pattern(R"((\w+)-(\d+))", boost::regex::nosubs);
    const std::string s = "item-42";
    boost::smatch m;

    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(pattern.mark_count(), 0u);
    EXPECT_EQ(m.size(), 1u);
    EXPECT_EQ(m[0].str(), "item-42");
}

TEST(RegexMatchFlags, ContinuousSearchStartsAtBeginningOnly) {
    const boost::regex pattern(R"(\d+)");
    boost::smatch m;
    const auto s = std::string("123abc");
    ASSERT_TRUE(boost::regex_search(
        s,
        m,
        pattern,
        boost::regex_constants::match_continuous));
    EXPECT_EQ(m[0].str(), "123");

    EXPECT_FALSE(boost::regex_search(
        std::string("abc123"),
        m,
        pattern,
        boost::regex_constants::match_continuous));
}