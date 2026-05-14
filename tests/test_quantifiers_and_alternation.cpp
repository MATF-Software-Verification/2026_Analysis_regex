#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(RegexQuantifiers, ZeroOrMore) {
    boost::regex pattern(R"(ab*c)");
    EXPECT_TRUE(boost::regex_match("ac", pattern));
    EXPECT_TRUE(boost::regex_match("abc", pattern));
    EXPECT_TRUE(boost::regex_match("abbc", pattern));
    EXPECT_FALSE(boost::regex_match("aXc", pattern));
}

TEST(RegexQuantifiers, OneOrMore) {
    boost::regex pattern(R"(ab+c)");
    EXPECT_FALSE(boost::regex_match("ac", pattern));
    EXPECT_TRUE(boost::regex_match("abc", pattern));
    EXPECT_TRUE(boost::regex_match("abbc", pattern));
}

TEST(RegexQuantifiers, ZeroOrOne) {
    boost::regex pattern(R"(ab?c)");
    EXPECT_TRUE(boost::regex_match("ac", pattern));
    EXPECT_TRUE(boost::regex_match("abc", pattern));
    EXPECT_FALSE(boost::regex_match("abbc", pattern));
}

TEST(RegexQuantifiers, ExactCount) {
    boost::regex pattern(R"(ab{2}c)");
    EXPECT_FALSE(boost::regex_match("abc", pattern));
    EXPECT_TRUE(boost::regex_match("abbc", pattern));
    EXPECT_FALSE(boost::regex_match("abbbc", pattern));
}

TEST(RegexQuantifiers, LowerBound) {
    boost::regex pattern(R"(ab{2,}c)");
    EXPECT_FALSE(boost::regex_match("abc", pattern));
    EXPECT_TRUE(boost::regex_match("abbc", pattern));
    EXPECT_TRUE(boost::regex_match("abbbc", pattern));
}

TEST(RegexQuantifiers, RangeCount) {
    boost::regex pattern(R"(ab{2,4}c)");
    EXPECT_FALSE(boost::regex_match("abc", pattern));
    EXPECT_TRUE(boost::regex_match("abbc", pattern));
    EXPECT_TRUE(boost::regex_match("abbbc", pattern));
    EXPECT_TRUE(boost::regex_match("abbbbc", pattern));
    EXPECT_FALSE(boost::regex_match("abbbbbc", pattern));
}

TEST(RegexQuantifiers, LazyQuantifier) {
    boost::regex pattern(R"(foo.*?foo)");
    std::string s = "fooaaafoobbbfoo";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[0].str(), "fooaaafoo");
}

TEST(RegexQuantifier, PossessiveQuantifier) {
    boost::regex pattern(R"(a++[ab])");
    EXPECT_FALSE(boost::regex_match("aaa", pattern)); // doesnt backtrack
    EXPECT_TRUE(boost::regex_match("aab", pattern));
}

TEST(RegexAlternation, SimpleAlternation) {
    boost::regex pattern(R"(cat|dog)");
    EXPECT_TRUE(boost::regex_match("cat", pattern));
    EXPECT_TRUE(boost::regex_match("dog", pattern));
    EXPECT_FALSE(boost::regex_match("mouse", pattern));
}