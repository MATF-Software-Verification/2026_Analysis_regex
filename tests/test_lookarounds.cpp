#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

// #### Lookarounds ####

TEST(RegexLookarounds, PositiveLookahead) {
    const boost::regex pattern(R"(\w+(?=\s))");
    const std::string s = "hello world";
    boost::smatch m;

    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[0].str(), "hello");
    EXPECT_EQ(m.suffix().str(), " world");
}

TEST(RegexLookarounds, NegativeLookahead) {
    const boost::regex pattern(R"(a(?!b(?!c)))");
    
    EXPECT_TRUE(boost::regex_search(std::string("a"), pattern));
    EXPECT_TRUE(boost::regex_search(std::string("ac"), pattern));
    EXPECT_FALSE(boost::regex_search(std::string("ab"), pattern));
    EXPECT_FALSE(boost::regex_search(std::string("abd"), pattern));
    EXPECT_FALSE(boost::regex_search(std::string("abc"), pattern));
}

TEST(RegexLookarounds, PositiveLookbehind) {
    const boost::regex pattern(R"((?<=foo)bar)");
    const std::string s = "foobarbarfoo";
    boost::smatch m;

    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[0].str(), "bar");
    EXPECT_EQ(m.prefix().str(), "foo");
    EXPECT_EQ(m.suffix().str(), "barfoo");
}

TEST(RegexLookarounds, NegativeLookbehind) {
    const boost::regex pattern(R"((?<!foo)bar)");
    const std::string s = "foobarbarfoo";
    boost::smatch m;

    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[0].str(), "bar");
    EXPECT_EQ(m.prefix().str(), "foobar");
    EXPECT_EQ(m.suffix().str(), "foo");
}

// #### Atomic groups ####

TEST(RegexAtomicGroups, NoBacktrackingAcrossAlternatives) {
    boost::regex pattern(R"(a(bc|b)c)");
    boost::regex patternAtomic(R"(a(?>bc|b)c)");

    EXPECT_TRUE(boost::regex_match(std::string("abcc"), pattern));
    EXPECT_TRUE(boost::regex_match(std::string("abc"), pattern));

    EXPECT_TRUE(boost::regex_match(std::string("abcc"), patternAtomic));   
    EXPECT_FALSE(boost::regex_match(std::string("abc"), patternAtomic));   
}

TEST(RegexAtomicGroups, NoBacktrackingAfterGreedyQuantifier) {
    const boost::regex pattern(R"((\d+)\d)");
    const boost::regex patternAtomic(R"((?>\d+)\d)");

    EXPECT_TRUE(boost::regex_match(std::string("123"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("123"), patternAtomic));
}

TEST(RegexAtomicGroups, NoBacktrackingAfterLazyQuantifier) {
    const boost::regex pattern(R"((\d+?)\d)");
    const boost::regex patternAtomic(R"((?>\d+?)\d)");

    EXPECT_TRUE(boost::regex_match(std::string("123"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("123"), patternAtomic));
}

TEST(RegexAtomicGroups, CaptureGroupsInsideAtomicGroup) {
    const boost::regex pattern(R"((?>(foo|bar))baz)");
    const auto s = std::string("foobaz");
    boost::smatch m;

    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    ASSERT_EQ(m.size(), 2u);
    EXPECT_EQ(m[0].str(), "foobaz");
    EXPECT_EQ(m[1].str(), "foo");
}

TEST(RegexAtomicGroups, NestedAtomicGroups) {
    const boost::regex pattern(R"((?>(?>a|b)c)d)");
    const auto s = std::string("acd");
    boost::smatch m;

    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[0].str(), "acd");
}

TEST(RegexAtomicGroups, MatchingAtomicAlternatives) {
    const boost::regex pattern(R"((?>ab|a)c)");
    EXPECT_TRUE(boost::regex_match(std::string("abc"), pattern));
    EXPECT_TRUE(boost::regex_match(std::string("ac"), pattern));
}