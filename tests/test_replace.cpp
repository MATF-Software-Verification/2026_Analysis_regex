#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

// #### Replace ####


TEST(RegexReplace, ConstructionOverloadsReplacesAllOccurances) {
    const std::string input_str = "id=123 foo=42 bar=foo";
    const std::string result_str = "id:[123] foo:[42] bar=foo";
    
    const boost::regex pattern(R"((\w+)=(\d+))");
    
    const std::string result = boost::regex_replace(input_str, pattern, "$1:[$2]");
    
    EXPECT_EQ(result, result_str);

    std::string output;
    boost::regex_replace(std::back_inserter(output), input_str.begin(), input_str.end(), pattern, "$1:[$2]");

    EXPECT_EQ(output, result_str);
}

TEST(RegexReplace, OutputIteratorOverloadReturnsIterator) {
    const std::string input_str = "a1 b2";
    const std::string result_str = "a-1 b-2";
    
    const boost::regex pattern(R"(([a-z])(\d))");
    
    char output[32] = {};
    char* returned = boost::regex_replace(
        output, 
        input_str.begin(), 
        input_str.end(), 
        pattern, 
        "$1-$2");

    *returned = '\0';

    EXPECT_STREQ(result_str.c_str(), output);
    EXPECT_EQ(output + result_str.size(), returned);
}

TEST(RegexReplace, ReturnsInputWhenNoMatchFound) {
    const std::string input_str = "no numbers here";
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(input_str, pattern, "X");

    EXPECT_EQ(input_str, result);
}

TEST(RegexReplace, ReturnsEmptyOutputWhenNoMatchFoundAndFormatNoCopy) {
    const std::string input_str = "no numbers here";
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(input_str, pattern, "X", boost::regex_constants::format_no_copy);

    EXPECT_TRUE(result.empty());
}

TEST(RegexReplace, FormatNoCopyOmitsUnmatchedText) {
    const std::string input_str = "id=123 foo=42 bar=foo";
    const std::string result_str = "id:[123]foo:[42]";
    
    const boost::regex pattern(R"((\w+)=(\d+))");
    
    const std::string result = boost::regex_replace(input_str, pattern, "$1:[$2]", boost::regex_constants::format_no_copy);
    
    EXPECT_EQ(result, result_str);
}

TEST(RegexReplace, FormatFirstOnlyReplacesFirstMatch) {
    const std::string input_str = "id=123 foo=42 bar=foo";
    const std::string result_str = "id:[123] foo=42 bar=foo";
    
    const boost::regex pattern(R"((\w+)=(\d+))");
    
    const std::string result = boost::regex_replace(input_str, pattern, "$1:[$2]", boost::regex_constants::format_first_only);
    
    EXPECT_EQ(result, result_str);
}

TEST(RegexReplace, FormatFirstOnlyFormatNoCopyReturnsOnlyFirstReplacement) {
        const std::string input_str = "id=123 foo=42 bar=foo";
    const std::string result_str = "id:[123]";
    
    const boost::regex pattern(R"((\w+)=(\d+))");
    
    const std::string result = boost::regex_replace(
        input_str, 
        pattern, 
        "$1:[$2]", 
        boost::regex_constants::format_first_only | boost::regex_constants::format_no_copy);
    
    EXPECT_EQ(result, result_str);
}

TEST(RegexReplace, SupportsWholeMatchReference) {
    const std::string input_str = "a b c";
    const boost::regex pattern(R"([a-z])");

    const std::string result = boost::regex_replace(input_str, pattern, "<$&>");

    EXPECT_EQ(result, "<a> <b> <c>");
}

TEST(RegexReplace, StringFormatter) {
    const std::string input_str = "id=123";
    const boost::regex pattern(R"((\w+)=(\d+))");
    const std::string formatter = "$1:[$2]";

    const std::string result = boost::regex_replace(input_str, pattern, formatter);

    EXPECT_EQ(result, "id:[123]");
}

TEST(RegexReplace, CStringFormatter) {
    const std::string input_str = "id=123";
    const boost::regex pattern(R"((\w+)=(\d+))");
    const char* formatter = "$1:[$2]";

    const std::string result = boost::regex_replace(input_str, pattern, formatter);

    EXPECT_EQ(result, "id:[123]");
}

TEST(RegexReplace, ConditionalFormatterWithFormatAll) {
        const std::string input_str = "id=123";
    const boost::regex pattern(R"((\w+)=(\d+))");
    const char* formatter = R"((?{1}$1:foo):(?{2}[$2]:[bar]))";

    const std::string result = boost::regex_replace(input_str, pattern, formatter, boost::regex_constants::format_all);

    EXPECT_EQ(result, "id:[123]");
}

TEST(RegexReplace, SedFormatter)
{
    const std::string input_str = "2001-01-01";
    const boost::regex pattern(R"((\d{4})-(\d{2})-(\d{2}))");

    const std::string result = boost::regex_replace(
        input_str,
        pattern,
        R"(\3.\2\.\1.)",
        boost::regex_constants::format_sed);

    EXPECT_EQ(result, "01.01.2001.");
}

TEST(RegexReplace, FunctorFormatter) {
    const std::string input_str = "id=123 foo=42";
    const boost::regex pattern(R"((\w+)=(\d+))");

    const auto formatter = [](const boost::smatch& match) {
        return match[1].str() + ":[" + match[2].str() + "]";
    };

    const std::string result = boost::regex_replace(input_str, pattern, formatter);

    EXPECT_EQ(result, "id:[123] foo:[42]");
}

TEST(RegexReplace, CaseInsensitiveSearch) {
    const std::string input_str = "boost BOOST Boost";
    const boost::regex pattern(R"(boost)", boost::regex_constants::icase);

    const std::string result = boost::regex_replace(input_str, pattern, "X");

    EXPECT_EQ("X X X", result);
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


TEST(RegexReplace, PrefixMatchAndSuffixTokensCanBeUsedInReplacement) {
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(
        std::string("abc123def"),
        pattern,
        std::string("<$`|$&|$'>"));

    EXPECT_EQ(result, "abc<abc|123|def>def");
}

TEST(RegexReplace, EmptyFromatterRemovesMatches) {
    const boost::regex pattern(R"(\d+)");

    const std::string result = boost::regex_replace(
        std::string("room 101, floor 7"),
        pattern,
        std::string(""));

    EXPECT_EQ(result, "room , floor ");
}