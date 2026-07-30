#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(RegexFlags, CaseInsensitiveMatching) {
    const boost::regex pattern("hello", boost::regex::icase);

    EXPECT_TRUE(boost::regex_match(std::string("HeLLo"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("world"), pattern));
}

TEST(RegexFlags, NoSubsSuppressesCaptureGroups) {
    const boost::regex pattern(R"((\w+)-(\d+))", boost::regex::nosubs);
    const std::string s = "item-42";
    boost::smatch m;

    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(pattern.mark_count(), 0u);
    EXPECT_EQ(m.size(), 1u);
    EXPECT_EQ(m[0].str(), "item-42");
    EXPECT_FALSE(m[1].matched);

}


TEST(RegexFlags, ContinuousSearchStartsAtBeginningOnly) {
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
    EXPECT_FALSE(m[0].matched);
}

TEST(RegexFlags, NotNullRejectsEmptyMatch) {
    const boost::regex pattern(R"(\d*)");
    boost::smatch match;
    const std::string input = "abc";

    ASSERT_TRUE(boost::regex_search(input, match, pattern));
    EXPECT_TRUE(match[0].str().empty());

    EXPECT_FALSE(boost::regex_search(
        input,
        pattern,
        boost::regex_constants::match_not_null));
}

TEST(RegexFlags, NotBolPreventsStartAnchorMatching) {
    const boost::regex pattern(R"(^abc)");

    EXPECT_FALSE(boost::regex_search(
        std::string("abc"),
        pattern,
        boost::regex_constants::match_not_bol));
}

TEST(RegexFlags, NotEolPreventsEndAnchorMatching) {
    const boost::regex pattern(R"(abc$)");

    EXPECT_FALSE(boost::regex_search(
        std::string("abc"),
        pattern,
        boost::regex_constants::match_not_eol));
}

TEST(RegexFlags, MatchNotBobPreventsBufferAnchorAtSubRangeStart)
{
    const std::string s = "xxxabc";

    const std::string::const_iterator first = s.begin() + 3;
    const std::string::const_iterator last = s.end();

    const boost::regex pattern(R"(\Aabc)");

    EXPECT_TRUE(boost::regex_search(first, last, pattern));

    EXPECT_FALSE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_not_bob));
}

TEST(MatchFlagType, MatchNotEobPreventsBufferAnchorAtSubRangeEnd)
{
    const std::string s = "abcxxx";

    const std::string::const_iterator first = s.begin();
    const std::string::const_iterator last = s.begin() + 3;

    const boost::regex pattern(R"(abc\z)");

    EXPECT_TRUE(boost::regex_search(first, last, pattern));

    EXPECT_FALSE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_not_eob));
}

TEST(RegexFlags, NotBowPreventsWordBoundaryAtStart) {
    const boost::regex pattern(R"(\bcat)");
    const std::string s = "cat naps";
    
    EXPECT_TRUE(boost::regex_search(s, pattern));
    EXPECT_FALSE(boost::regex_search(
        s, 
        pattern, 
        boost::regex_constants::match_not_bow));
}

TEST(RegexFlags, NotEowPreventsWordBoundaryAtEnd) {
    const boost::regex pattern(R"(cat\b)");
    const std::string s = "sleepy cat";
    EXPECT_TRUE(boost::regex_search(s, pattern));
    EXPECT_FALSE(boost::regex_search(
        s,
        pattern,
        boost::regex_constants::match_not_eow));
}

TEST(RegexFlags, PrevAvailCheckPreviousCharacter) {
    const boost::regex pattern_boundary(R"(\bcat)");
    const boost::regex pattern_anchor(R"(^cat)");
    const std::string input = "scat";
    const auto first = input.begin() + 1;

    ASSERT_TRUE(boost::regex_search(first, input.end(), pattern_boundary));
    ASSERT_TRUE(boost::regex_search(first, input.end(), pattern_anchor));

    EXPECT_FALSE(boost::regex_search(
        first,
        input.end(),
        pattern_boundary,
        boost::regex_constants::match_prev_avail));

    EXPECT_FALSE(boost::regex_search(
        first,
        input.end(),
        pattern_anchor,
        boost::regex_constants::match_prev_avail));
}

TEST(RegexFlags, PrevAvailMakesMatchNotBolIgnored) {
    const std::string s = "\nabc";

    const std::string::const_iterator first = s.begin() + 1;
    const std::string::const_iterator last = s.end();

    const boost::regex pattern(R"(^abc)");

    EXPECT_TRUE(boost::regex_search(first, last, pattern));

    EXPECT_FALSE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_not_bol));

    EXPECT_TRUE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_prev_avail | boost::regex_constants::match_not_bol));
}

TEST(RegexFlags, PrevAvailMakesMatchNotBowIgnored) {
    const std::string s = " abc";

    const std::string::const_iterator first = s.begin() + 1;
    const std::string::const_iterator last = s.end();

    const boost::regex pattern(R"(\babc)");

    EXPECT_TRUE(boost::regex_search(first, last, pattern));

    EXPECT_FALSE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_not_bow));

    EXPECT_TRUE(boost::regex_search(
        first,
        last,
        pattern,
        boost::regex_constants::match_prev_avail | boost::regex_constants::match_not_bow));
}