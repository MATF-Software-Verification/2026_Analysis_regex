#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>   

// #### Character Classes ####

TEST(RegexCharClasses, DigitClass) {
    boost::regex pattern(R"(^\d+)"); 
    EXPECT_TRUE(boost::regex_match("12345", pattern));
    EXPECT_FALSE(boost::regex_match("abc123", pattern));
}

TEST(RegexCharClasses, WordClass) {
    boost::regex pattern(R"(^\w+)"); 
    EXPECT_TRUE(boost::regex_match("hello_world", pattern));
    EXPECT_FALSE(boost::regex_match("hello world", pattern)); // space 
}

TEST(RegexCharClasses, WhitespaceClass) {
    boost::regex pattern(R"(^\s+$)");
    EXPECT_TRUE(boost::regex_match("\n    \t", pattern));
    EXPECT_FALSE(boost::regex_match("  a  ", pattern));
}

TEST(RegexCharClasses, NotDigitClass) {
    boost::regex pattern(R"(^\D+)");
    EXPECT_TRUE(boost::regex_match("abc", pattern));
    EXPECT_FALSE(boost::regex_match("abc123", pattern));
}

TEST(RegexCharClasses, NotWordClass) {
    boost::regex pattern(R"(^\W+)");
    EXPECT_TRUE(boost::regex_match("!@#$", pattern));
    EXPECT_FALSE(boost::regex_match("hello!", pattern));
}

TEST(RegexCharClasses, NotWhitespaceClass) {
    boost::regex pattern(R"(^\S+$)");
    EXPECT_TRUE(boost::regex_match("helloworld", pattern));
    EXPECT_FALSE(boost::regex_match("hello world", pattern));
}

TEST(RegexCharClasses, AnyOf) {
    boost::regex pattern(R"([abc]+)");
    EXPECT_TRUE(boost::regex_match("aaaabbcccaabb", pattern));
    EXPECT_FALSE(boost::regex_match("hello", pattern));
}

TEST(RegexCharClasses, NotAnyOf) {
    boost::regex pattern(R"([^abc]+)");
    EXPECT_TRUE(boost::regex_match("hello", pattern));
    EXPECT_FALSE(boost::regex_match("abc", pattern));
}

TEST(RegexCharClasses, CharactersBetween) {
    boost::regex pattern(R"([a-c]+)");
    EXPECT_TRUE(boost::regex_match("aaabbccb", pattern));
    EXPECT_FALSE(boost::regex_match("AAABBCCB", pattern)); // uppercase
}

// #### Literals and Wildcards ####

TEST(RegexLiterals, EscapedMetacharacters) {
    const boost::regex pattern(R"(^\$\d+\.\d{2}\?$)");

    EXPECT_TRUE(boost::regex_match(std::string("$12.50?"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("12.50?"), pattern));
}

TEST(RegexWildcard, DotDoesNotMatchNewline) {
    const boost::regex pattern(R"(a.b)");

    EXPECT_FALSE(boost::regex_search(std::string("a\nb"), pattern, boost::regex_constants::match_not_dot_newline));
    EXPECT_TRUE(boost::regex_search(std::string("a\nb"), pattern));
}
