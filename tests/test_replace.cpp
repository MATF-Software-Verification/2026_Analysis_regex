#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

// #### Replace ####

TEST(RegexReplace, SimpleReplace) {
    boost::regex pattern(R"(\d+)");
    std::string result = boost::regex_replace(std::string("foo 42 bar 7"), pattern, "NUM");
    EXPECT_EQ(result, "foo NUM bar NUM");
}

TEST(RegexReplace, ReplaceWithCaptureGroup) {
    boost::regex pattern(R"((\w+)=(\d+))");
    std::string result = boost::regex_replace(
        std::string("x=1 y=2 z=3"),
        pattern,
        "[$1 equals $2]"
    );
    EXPECT_EQ(result, "[x equals 1] [y equals 2] [z equals 3]");
}

TEST(RegexReplace, ReplaceNoMatch) {
    boost::regex pattern(R"(\d+)");
    std::string input = "no numbers here";
    std::string result = boost::regex_replace(input, pattern, "NUM");
    EXPECT_EQ(result, input);
}

TEST(RegexReplace, ReplaceFirstOnly) {
    boost::regex pattern(R"(\d+)");
    std::string result = boost::regex_replace(
        std::string("1 2 3"),
        pattern,
        "X",
        boost::regex_constants::format_first_only
    );
    EXPECT_EQ(result, "X 2 3");
}

TEST(RegexReplace, ReplaceWithEmptyString) {
    boost::regex pattern(R"(\s+)");
    std::string result = boost::regex_replace(std::string("a b c d"), pattern, "");
    EXPECT_EQ(result, "abcd");
}

TEST(RegexReplace, ReplaceEntireMatch) {
    boost::regex pattern(R"(^\w+)");
    std::string result = boost::regex_replace(std::string("hello world"), pattern, "goodbye");
    EXPECT_EQ(result, "goodbye world");
}

TEST(RegexReplace, FormatLiteralKeepsBackreferencesAsPlainText) {
    const boost::regex pattern(R"((\w+)=(\d+))");

    const std::string result = boost::regex_replace(
        std::string("x=1 y=2"),
        pattern,
        std::string("[$1:$2]"),
        boost::regex_constants::format_literal);

    EXPECT_EQ(result, "[$1:$2] [$1:$2]");
}

TEST(RegexReplace, FormatNoCopyReturnsOnlyReplacementText) {
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(
        std::string("a1b22c"),
        pattern,
        std::string("X"),
        boost::regex_constants::format_no_copy);

    EXPECT_EQ(result, "XX");
}

TEST(RegexReplace, PrefixMatchAndSuffixTokensCanBeUsedInReplacement) {
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(
        std::string("abc123def"),
        pattern,
        std::string("<$`|$&|$'>"));

    EXPECT_EQ(result, "abc<abc|123|def>def");
}

TEST(RegexReplace, LiteralAndNoCopyFlagsCanBeCombined) {
    const boost::regex pattern(R"(\d)");

    const std::string result = boost::regex_replace(
        std::string("a1b2"),
        pattern,
        std::string("$&"),
        boost::regex_constants::format_literal | boost::regex_constants::format_no_copy);

    EXPECT_EQ(result, "$&$&");
}

TEST(RegexReplace, EmptyReplacementRemovesMatchesOnly) {
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(
        std::string("room 101, floor 7"),
        pattern,
        std::string(""));

    EXPECT_EQ(result, "room , floor ");
}