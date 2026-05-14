#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

// #### Full Match ####

TEST(RegexMatch, SimpleMatchSuccess) {
    boost::regex pattern(R"(\d{3}-\d{4})"); 
    EXPECT_TRUE(boost::regex_match("123-4567", pattern));
}

TEST(RegexMatch, SimpleMatchFailure) {
    boost::regex pattern(R"(\d{3}-\d{4})");
    EXPECT_FALSE(boost::regex_match("12-4567", pattern));
}

TEST(RegexMatch, MatchEntireString) {
    boost::regex pattern(R"(\d+)");
    EXPECT_FALSE(boost::regex_match("123abc", pattern));
}


// #### Search ####

TEST(RegexSearch, MatchSubstringSuccess) {
    boost::regex pattern(R"(\d+)");
    EXPECT_TRUE(boost::regex_search("foo 42 test", pattern));
}

TEST(RegexSearch, MatchSubstringFailure) {
    boost::regex pattern(R"(\d+)");
    EXPECT_FALSE(boost::regex_search("no digits test", pattern));
}

TEST(RegexSearch, StringMatchCapturedGroups) {
    boost::regex pattern(R"((\w+)@(\w+)\.(\w+))");
    std::string s = "email: user@example.com";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[0].str(), "user@example.com");
    EXPECT_EQ(m[1].str(), "user");
    EXPECT_EQ(m[2].str(), "example");
    EXPECT_EQ(m[3].str(), "com");
}

// #### Anchors and Word Boundaries ####

TEST(RegexAnchors, WordBoundary) {
    boost::regex pattern(R"(\bcat\b)");
    EXPECT_TRUE(boost::regex_search(std::string("I feed my cat every morning"), pattern));
    EXPECT_FALSE(boost::regex_search(std::string("The dog scattered the food all over the floor"), pattern));
}

TEST(RegexAnchors, StartEndAnchors) {
    boost::regex pattern(R"(^\s*$)");
    EXPECT_TRUE(boost::regex_match("    ", pattern));
    EXPECT_FALSE(boost::regex_match("   s", pattern));
}

TEST(RegexAnchors, MultilineAnchors) {
    // Make start/end string anchors match individual lines rather than whole string
    boost::regex pattern(R"((?m)^\d+$)");

    std::string s1 = "abc\n123\nfoo";
    EXPECT_TRUE(boost::regex_search(s1, pattern));

    std::string s2 = "abc\nfoo";
    EXPECT_FALSE(boost::regex_search(s2, pattern));
}


