#include <boost/regex.hpp>
#include <fuzztest/fuzztest.h>
#include <gtest/gtest.h>


const std::vector<std::tuple<std::string, std::string>> regex_seeds = {
    {"", ""},
    {"a", "a"},
    {"a", "ba"},
    {"a", "b"},
    {R"(\d{3}-\d{4})", "123-4567"},
    {R"(\d{3}-\d{4})", "Phone: 123-4567"},
    {R"(^\d{3}-\d{4}$)", "123-4567"},
    {R"(^[a-zA-Z0-9_]+@[a-zA-Z0-9]+\.[a-z]{2,}$)", "user@example.com"},
    {R"((?<name>\w+)\s+(?<age>\d+))", "John 25"},
    {R"((a|b|c){2,5})", "aabbc"},
    {R"(a*)", ""},
    {R"(a+)", "aaaa"},
    {R"([a-z]+)", "abc"},
    {R"([a-z]+)", "123abc456"},
    {R"((ab)+)", "abab"},
    {R"(\bword\b)", "word"},
    {R"(\bword\b)", "a word here"},
    {R"((a+)+$)", "aaaaaaaa!"}
};

void MatchPattern(std::string pattern_str, std::string input_str) {
    try {
        boost::regex pattern(pattern_str);
        boost::regex_match(input_str, pattern);
    } catch(const boost::regex_error&) {
        return;
    }
}

FUZZ_TEST(RegexFuzz, MatchPattern)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(64),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128))
    .WithSeeds(regex_seeds);

void RegexGeneratedInputAlwaysMatches(const std::string& input) {
    const boost::regex pattern(R"([a-zA-Z0-9]+@[a-zA-Z0-9]+\.[a-z]{2,6})");

    EXPECT_TRUE(boost::regex_match(input, pattern));
}

FUZZ_TEST(RegexFuzz, RegexGeneratedInputAlwaysMatches)
    .WithDomains(fuzztest::InRegexp(R"([a-zA-Z0-9]+@[a-zA-Z0-9]+\.[a-z]{2,6})"));

void FullMatchImpliesSearch(const std::string& pattern_str, const std::string& input_str) {
    try {
        const boost::regex pattern(pattern_str);

        const bool full_match = boost::regex_match(input_str, pattern);
        const bool search_match = boost::regex_search(input_str, pattern);

        EXPECT_FALSE(full_match && !search_match);
    } catch (const boost::regex_error&) {
        return;
}
}

FUZZ_TEST(RegexFuzz, FullMatchImpliesSearch)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(64),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128)
    )
    .WithSeeds(regex_seeds);

void ReplaceWithFormat(std::string pattern_str, std::string input_str, std::string format_str) {
    try {
        const boost::regex pattern(pattern_str);

        const std::string direct_result = boost::regex_replace(input_str, pattern, format_str);

        std::string iterator_result;

        boost::regex_replace(std::back_inserter(iterator_result), input_str.begin(), input_str.end(), pattern, format_str);

        EXPECT_EQ(direct_result, iterator_result);

        const bool contains_match = boost::regex_search(input_str, pattern);

        if (!contains_match) {
            EXPECT_EQ(direct_result, input_str);
        }
    } catch(const boost::regex_error&) {
        return;
    }
}

FUZZ_TEST(RegexFuzz, ReplaceWithFormat)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(64),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128),
        fuzztest::Arbitrary<std::string>().WithMaxSize(64)
    )
    .WithSeeds({
        {"", "", ""},
        {"", "abc", "-"},
        {R"(\d+)", "abc", "#"},
        {R"(\d+)", "a12b345", "<$&>"},
        {R"((\w+)\s+(\w+))", "John Smith", "$2, $1"},
        {R"((a)?b)", "b ab", "[$1]"},
        {R"((?<word>\w+))", "abc 123", "$+{word}"},
        {R"([aeiou])", "boost regex", "$$"},
        {R"(\s+)", "a   b	c", "_"},
        {R"(^|$)", "abc", "|"}
    });

void IteratorWalk(std::string pattern_str, std::string input_str) {
    try {
        boost::regex pattern(pattern_str);
        boost::sregex_iterator it(input_str.begin(), input_str.end(), pattern);
        boost::sregex_iterator end;
        int count = 0;
        for(; it != end && count < 1000; ++it, ++count) {
            ASSERT_TRUE(it->empty() == false);
        }
    } catch(const boost::regex_error&) {
        return;
    }
}
FUZZ_TEST(RegexFuzz, IteratorWalk)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(64),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128)
    )
    .WithSeeds({
        {"", ""},
        {"", "abc"},
        {R"(\d+)", "a1 bb22 c333"},
        {R"([a-z]+)", "123abc456def"},
        {R"((a)(b)?)", "a ab abb"},
        {R"(a*)", "bbb"},
        {R"(a*)", "aa"},
        {R"((?=a))", "aaa"},
        {R"(\b)", "one two"},
        {R"(^|$)", "abc"},
        {R"(aba)", "ababa"},
        {R"((a|aa)+)", "aaaa"}
    });
