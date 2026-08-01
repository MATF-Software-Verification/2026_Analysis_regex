#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>


TEST(BasicRegex, BasicRegexDefaultConstructionAndAssign) {
    boost::regex pattern;

    EXPECT_TRUE(pattern.empty());
    EXPECT_EQ(pattern.status(), boost::regex_constants::error_empty);
    EXPECT_EQ(pattern.size(), 0u);
    EXPECT_EQ(pattern.mark_count(), 0u);
    EXPECT_EQ(pattern.error_code(), boost::regex_constants::error_empty);
    EXPECT_EQ(pattern.str(), "");
    EXPECT_EQ(pattern.begin(), nullptr);
    EXPECT_EQ(pattern.end(), nullptr);
    EXPECT_EQ(pattern.expression(), nullptr);
    EXPECT_EQ(pattern.max_size(), static_cast<boost::regex::size_type>(UINT_MAX));

    const std::string re_str = R"((ab)cd)";
    pattern.assign(re_str, boost::regex_constants::perl);

    EXPECT_FALSE(pattern.empty());
    EXPECT_EQ(pattern.status(), 0);
    EXPECT_EQ(pattern.size(), pattern.size());
    EXPECT_EQ(pattern.mark_count(), 1u);
    EXPECT_EQ(pattern.flags(), boost::regex_constants::perl);
    EXPECT_EQ(pattern.str(), re_str);
    EXPECT_TRUE(boost::regex_match(std::string("abcd"), pattern));
}

TEST(BasicRegex, BasicRegexConstructFromIteratorRangeAndPointerRange) {
    const std::string pattern = R"(foo-(\d{2}))";
    const boost::regex from_iterators(pattern.begin(), pattern.end());
    const boost::regex from_pointers(pattern.data(), pattern.data() + pattern.size());
    const boost::regex from_pointer(pattern.data(), pattern.size(), boost::regex_constants::normal);

    

    boost::smatch iterators_match;
    boost::smatch pointers_match;
    boost::smatch pointer_match;

    std::string input_str = "foo-42";
    ASSERT_TRUE(boost::regex_match(input_str, iterators_match, from_iterators));
    ASSERT_TRUE(boost::regex_match(input_str, pointers_match, from_pointers));
    ASSERT_TRUE(boost::regex_match(input_str, pointer_match, from_pointer));

    EXPECT_EQ(iterators_match[1].str(), "42");
    EXPECT_EQ(pointers_match[1].str(), "42");
    EXPECT_EQ(pointer_match[1].str(), "42");
    EXPECT_EQ(from_iterators.str(), from_pointers.str());
    EXPECT_EQ(from_pointers.str(), from_pointer.str());
}

TEST(BasicRegex, BasicRegexCopyCompareAssignOp) {
    const std::string pattern = R"([A-Za-z]+)";
    const boost::regex words(pattern);
    const boost::regex words_copy(words);
    boost::regex assigned;
    assigned = words;

    EXPECT_EQ(words, words_copy);
    EXPECT_EQ(words.compare(assigned), 0); 

    const boost::regex words_flag(pattern, boost::regex_constants::perl);
    EXPECT_EQ(words.compare(words_flag), words.flags() - words_flag.flags());

    boost::regex digits(R"(\d+)");
    boost::regex letters(R"([A-Z]+)");
    digits.swap(letters);

    EXPECT_TRUE(boost::regex_match(std::string("ABC"), digits));
    EXPECT_TRUE(boost::regex_match(std::string("123"), letters));
}

TEST(BasicRegex, BasicRegexCompareOperatorsSwapAndStreams) {
    boost::regex pattern("abc");
    boost::regex pattern_eq("abc");
    boost::regex pattern_geq("abd");

    EXPECT_TRUE(pattern == pattern_eq);
    EXPECT_FALSE(pattern != pattern_eq);
    EXPECT_TRUE(pattern < pattern_geq);
    EXPECT_TRUE(pattern <= pattern_geq);
    EXPECT_TRUE(pattern_geq > pattern);
    EXPECT_TRUE(pattern_geq >= pattern);

    swap(pattern, pattern_geq);
    EXPECT_EQ(pattern.str(), "abd");
    EXPECT_EQ(pattern_geq.str(), "abc");

    std::ostringstream output;
    output << pattern;
    EXPECT_EQ(output.str(), "abd");
} 

TEST(BasicRegex, SetExpressionStoresStatus) {
    boost::regex pattern;

    const auto invalid_status = pattern.set_expression("(");
    EXPECT_EQ(invalid_status, boost::regex_constants::error_paren);
    EXPECT_EQ(pattern.status(), boost::regex_constants::error_paren);
    EXPECT_TRUE(pattern.empty());
    EXPECT_EQ(pattern.expression(), nullptr);

}

TEST(BasicRegex, Subexpression) {
    boost::regex pattern(R"((ab)(cd))", boost::regex_constants::save_subexpression_location);

    const auto capture1 = pattern.subexpression(0); //returns iterator pair
    const auto capture2 = pattern.subexpression(1);

    ASSERT_NE(capture1.first, nullptr);
    ASSERT_NE(capture1.second, nullptr);
    ASSERT_NE(capture2.first, nullptr);
    ASSERT_NE(capture2.second, nullptr);

    EXPECT_TRUE(capture1.first >= pattern.begin());
    EXPECT_TRUE(capture1.second <= pattern.end());
    EXPECT_TRUE(capture2.first >= pattern.begin());
    EXPECT_TRUE(capture2.second <= pattern.end());
    EXPECT_TRUE(capture1.first <= capture1.second);
    EXPECT_TRUE(capture2.first <= capture2.second);

    EXPECT_NE(std::string(capture1.first, capture1.second).find("ab"), std::string::npos);
    EXPECT_NE(std::string(capture2.first, capture2.second).find("cd"), std::string::npos);
}
