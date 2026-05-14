#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

TEST(RegexMatchFlagsExtended, NotNullRejectsEmptyMatch) {
    const boost::regex pattern(R"(\d*)");
    boost::smatch match;
    const std::string input = "abc";

    ASSERT_TRUE(boost::regex_search(input, match, pattern));
    EXPECT_TRUE(match[0].str().empty());

    EXPECT_FALSE(boost::regex_search(
        input,
        match,
        pattern,
        boost::regex_constants::match_not_null));
}

TEST(RegexMatchFlagsExtended, NotBolPreventsCaretFromMatchingAtStart) {
    const boost::regex pattern(R"(^abc)");
    boost::smatch match;

    EXPECT_FALSE(boost::regex_search(
        std::string("abc"),
        match,
        pattern,
        boost::regex_constants::match_not_bol));
}

TEST(RegexMatchFlagsExtended, NotEolPreventsDollarFromMatchingAtEnd) {
    const boost::regex pattern(R"(abc$)");
    boost::smatch match;

    EXPECT_FALSE(boost::regex_search(
        std::string("abc"),
        match,
        pattern,
        boost::regex_constants::match_not_eol));
}

TEST(RegexMatchFlagsExtended, NotBowPreventsWordBoundaryAtStart) {
    const boost::regex pattern(R"(\bcat)");
    boost::smatch match;

    EXPECT_FALSE(boost::regex_search(
        std::string("cat naps"),
        match,
        pattern,
        boost::regex_constants::match_not_bow));
}

TEST(RegexMatchFlagsExtended, NotEowPreventsWordBoundaryAtEnd) {
    const boost::regex pattern(R"(cat\b)");
    boost::smatch match;

    EXPECT_FALSE(boost::regex_search(
        std::string("sleepy cat"),
        match,
        pattern,
        boost::regex_constants::match_not_eow));
}

TEST(RegexMatchFlagsExtended, PrevAvailUsesCharacterBeforeSearchRangeForBoundaries) {
    const boost::regex pattern(R"(\bcat)");
    const std::string input = "scat";
    const auto first = input.cbegin() + 1;
    boost::match_results<std::string::const_iterator> match;

    ASSERT_TRUE(boost::regex_search(first, input.cend(), match, pattern));
    EXPECT_EQ(match[0].str(), "cat");

    EXPECT_FALSE(boost::regex_search(
        first,
        input.cend(),
        match,
        pattern,
        boost::regex_constants::match_prev_avail));
}