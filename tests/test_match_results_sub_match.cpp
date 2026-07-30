#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <cstddef>
#include <string>

// #### Match Results ####

TEST(MatchResults, SMatchDefaultConstruction) {
    const boost::smatch match;

    EXPECT_TRUE(match.empty());
    EXPECT_EQ(match.size(), 0u);
    EXPECT_THROW(static_cast<void>(match.str()), std::logic_error);
}

TEST(MatchResults, CMatchDefaultConstruction) {
    const boost::cmatch match;

    EXPECT_TRUE(match.empty());
    EXPECT_EQ(match.size(), 0u);
    EXPECT_THROW(static_cast<void>(match.str()), std::logic_error);
}

TEST(MatchResults, ExplicitDefaultConstruction) {
    typedef std::string::const_iterator IteratorType;

    const boost::match_results<IteratorType> match;

    EXPECT_TRUE(match.empty());
    EXPECT_EQ(match.size(), 0u);
    EXPECT_THROW(static_cast<void>(match.str()), std::logic_error);
}

TEST(MatchResults, EmptyObjectCopy) {
    const boost::smatch original;
    const boost::smatch original_copy(original);

    EXPECT_TRUE(original_copy.empty());
    EXPECT_EQ(original_copy.size(), 0u);
    EXPECT_EQ(original, original_copy);
}

TEST(MatchResults, AssignOperatorEmptyObjects) {
    const boost::smatch original;
    boost::smatch assigned;

    assigned = original;

    EXPECT_TRUE(assigned.empty());
    EXPECT_EQ(assigned.size(), 0u);
    EXPECT_EQ(original, assigned);
}

TEST(MatchResults, SwapEmptyObjects) {
    boost::smatch first;
    boost::smatch second;

    first.swap(second);

    EXPECT_TRUE(first.empty());
    EXPECT_TRUE(second.empty());
    EXPECT_EQ(first, second);
}

TEST(MatchResults, MatchResultsThrowsLogicError) {
    const boost::smatch match;

    EXPECT_THROW(static_cast<void>(match.length()), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.length(0)), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.length(1)), std::logic_error);

    EXPECT_THROW(static_cast<void>(match.position()), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.position(0uz)), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.position(1uz)), std::logic_error);

    EXPECT_THROW(static_cast<void>(match.str()), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.str(0)), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.str(1)), std::logic_error);

    EXPECT_THROW(static_cast<void>(match.length()), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.length(0)), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.length(1)), std::logic_error);

    EXPECT_THROW(static_cast<void>(match[0]), std::logic_error);
    EXPECT_THROW(static_cast<void>(match[1]), std::logic_error);

    EXPECT_THROW(static_cast<void>(match.prefix()), std::logic_error);
    EXPECT_THROW(static_cast<void>(match.suffix()), std::logic_error);

    EXPECT_THROW(static_cast<void>(match.format("$0")), std::logic_error);

}

TEST(MatchResults, RegexMatchInitializesCMatch) {
    const char* input_str = "prefix id=123 suffix";
    const boost::regex pattern(R"(id=(\d+))");
    boost::cmatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));
    
    EXPECT_FALSE(match.empty());
    ASSERT_EQ(match.size(), 2uz);
    EXPECT_EQ(match[0].str(), "id=123");
    EXPECT_EQ(match[1].str(), "123");
    EXPECT_EQ(match[0].str(), match.str(0));
    EXPECT_EQ(match.position(0uz), 7); //start position of match[0]
    EXPECT_EQ(match.position(3uz), -1);
    EXPECT_EQ(match.length(1), 3); 
    EXPECT_EQ(match.length(0), 6); 
    EXPECT_EQ(match.prefix(), std::string(input_str, match.position(0uz)));
    EXPECT_EQ(match.suffix(), std::string(input_str + match.position(0uz) + match.length(0)));
}

TEST(MatchResults, IteratorAccess) {
    const std::string input_str = "prefix id=123 key=value suffix";
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    std::vector<std::string> captures;

    for(boost::smatch::const_iterator it = match.begin(); it != match.end(); ++it) {
        captures.push_back(it->str());
    }

    ASSERT_EQ(captures.size(), 3u);
    EXPECT_EQ("id=123", captures[0]);
    EXPECT_EQ("id", captures[1]);
    EXPECT_EQ("123", captures[2]);
}

TEST(MatchResults, InitializedObjectsCopy) {
    const std::string input_str = "prefix id=123 key=value suffix";
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    boost::smatch match_copy(match);

    EXPECT_EQ(match, match_copy); 
}

TEST(MatchResults, AssignOperatorInitializedObjects) {
    boost::smatch original;
    boost::smatch assigned;

    boost::regex pattern(R"((\w+)@(\w+)\.(\w+))");
    std::string s = "email: user@example.com";
    ASSERT_TRUE(boost::regex_search(s, original, pattern));

    assigned = original;

    EXPECT_EQ(original.empty(), assigned.empty());
    EXPECT_EQ(original.size(), assigned.size());
    EXPECT_EQ(original.prefix(), assigned.prefix());
    EXPECT_EQ(original.suffix(), assigned.suffix());
    for(int i = 0; i < original.size(); i ++) {
        EXPECT_EQ(original[i], assigned[i]);
        EXPECT_EQ(original.str(i), assigned.str(i));
        EXPECT_EQ(original.length(i), assigned.length(i));
        EXPECT_EQ(original.position(i), assigned.position(i));
    }
}

TEST(MatchResults, InitializedObjectsSwap) {
    const std::string input_str1 = "prefix id=123 key=value suffix";
    const std::string input_str2 = "key=2";
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::smatch first;
    boost::smatch second;

    ASSERT_TRUE(boost::regex_search(input_str1, first, pattern));
    ASSERT_TRUE(boost::regex_search(input_str2, second, pattern));

    first.swap(second);

    EXPECT_EQ("key=2", first.str(0));
    EXPECT_EQ("key", first.str(1));
    EXPECT_EQ("2", first.str(2));

    EXPECT_EQ("id=123", second.str(0));
    EXPECT_EQ("id", second.str(1));
    EXPECT_EQ("123", second.str(2));

    swap(first, second);

    EXPECT_EQ("id=123", first.str(0));
    EXPECT_EQ("id", first.str(1));
    EXPECT_EQ("123", first.str(2));

    EXPECT_EQ("key=2", second.str(0));
    EXPECT_EQ("key", second.str(1));
    EXPECT_EQ("2", second.str(2));
}

TEST(MatchResults, NamedSubexpressionAccessByName) {
    const std::string input_str = "prefix id=123 key=value suffix";
    const boost::regex pattern(R"((?<key>\w+)=(?<value>\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    EXPECT_EQ("id", match.str("key"));
    EXPECT_EQ("123", match.str("value"));
    EXPECT_EQ(7, match.position("key"));
    EXPECT_EQ(10, match.position("value"));
    EXPECT_EQ(2, match.length("key"));
    EXPECT_EQ(3, match.length("value"));
}

TEST(MatchResults, MissingSubexpressions) {
    const std::string input_str = "prefix id=123 key=value suffix";
    const boost::regex pattern(R"((?<key>\w+)=(?<value>\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    EXPECT_FALSE(match[3].matched);
    EXPECT_EQ(match.str(3), "");
    EXPECT_EQ(match.length(3), 0);
    EXPECT_EQ(match.position(3), -1);

    EXPECT_FALSE(match["missing"].matched);
    EXPECT_EQ(match.str("missing"), "");
    EXPECT_EQ(match.length("missing"), 0);
    EXPECT_EQ(match.position("missing"), -1);
}

TEST(MatchResults, FormatCaptures) {
    const std::string input_str = "2001-01-01";
    const boost::regex pattern(R"((\d{4})-(\d{2})-(\d{2}))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    EXPECT_EQ("01.01.2001.", match.format("$3.$2.$1."));
}

TEST(MatchResults, FormatOutputIterator) {
    const std::string input_str = "2001-01-01";
    const boost::regex pattern(R"((\d{4})-(\d{2})-(\d{2}))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));
    
    std::string output;
    
    match.format(std::back_inserter(output), "$3.$2.$1.");
    EXPECT_EQ("01.01.2001.", output);
}

// #### Sub Match ####

TEST(SubMatch, CapturedSubMatchesIteratorRange) {
    const std::string input_str = "id=123";
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    const boost::ssub_match whole = match[0];
    const boost::ssub_match first_capture = match[1];
    const boost::ssub_match second_capture = match[2];

    EXPECT_TRUE(whole.matched);
    EXPECT_TRUE(first_capture.matched);
    EXPECT_TRUE(second_capture.matched);

    EXPECT_EQ(whole.str(), "id=123");
    EXPECT_EQ(first_capture.str(), "id");
    EXPECT_EQ(second_capture.str(), "123");

    EXPECT_EQ(whole.length(), 6);
    EXPECT_EQ(first_capture.length(), 2);
    EXPECT_EQ(second_capture.length(), 3);
    
    EXPECT_EQ(input_str.begin(), whole.first);
    EXPECT_EQ(input_str.end(), whole.second);
    EXPECT_EQ(input_str.begin(), first_capture.first);
    EXPECT_EQ(input_str.begin() + 2, first_capture.second);
    EXPECT_EQ(input_str.begin() + 3, second_capture.first);
    EXPECT_EQ(input_str.end(), second_capture.second);
}

TEST(SubMatch, OptionalUnmatchedSubMatch) {
    const std::string input_str = "abc";
    const boost::regex pattern(R"((abc)(\d)?)");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    const boost::ssub_match required = match[1];
    const boost::ssub_match optional = match[2];

    EXPECT_TRUE(required.matched);

    EXPECT_FALSE(optional.matched);
    EXPECT_EQ(optional.str(), "");
    EXPECT_EQ(optional.length(), 0);
}

TEST(SubMatch, PrefixAndSuffixSubMatchObjects) {
    const std::string input_str = "prefix id=123 suffix";
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    const boost::ssub_match prefix = match.prefix();
    const boost::ssub_match suffix = match.suffix();

    EXPECT_TRUE(prefix.matched);
    EXPECT_TRUE(suffix.matched);

    EXPECT_EQ(prefix.str(), "prefix ");
    EXPECT_EQ(suffix.str(), " suffix");
}

TEST(SubMatch, SubMatchCompareWithString) {
    const std::string input_str = "abc";
    const boost::regex pattern(R"((abc))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    const boost::ssub_match capture = match[1];

    EXPECT_EQ(capture.compare(std::string("abc")), 0);
    EXPECT_LT(capture.compare(std::string("abd")), 0);
    EXPECT_GT(capture.compare(std::string("abb")), 0);

    EXPECT_TRUE(capture == std::string(capture));
    EXPECT_TRUE(capture != std::string("aaa"));
    EXPECT_TRUE(capture <= std::string("abd"));
    EXPECT_TRUE(capture >= std::string("abb"));
    EXPECT_TRUE(capture < std::string("abd"));
    EXPECT_TRUE(capture > std::string("abb"));
}

TEST(SubMatch, SubMatchCompareWithCString)
{
    const std::string input_str = "abc";
    const boost::regex patterm(R"((abc))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, patterm));

    const boost::ssub_match capture = match[1];

    EXPECT_EQ(capture.compare("abc"), 0);
    EXPECT_LT(capture.compare("abd"), 0);
    EXPECT_GT(capture.compare("abb"), 0);

    EXPECT_TRUE(capture == "abc");
    EXPECT_TRUE(capture != "aaa");
    EXPECT_TRUE(capture <= "abd");
    EXPECT_TRUE(capture >= "abb");
    EXPECT_TRUE(capture < "abd");
    EXPECT_TRUE(capture > "abb");
}

TEST(SubMatch, SubMatchCompareWithSubMatch) {
    const std::string input_str = "abc:abb";
    const boost::regex patterm(R"((\w+):(\w+))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, patterm));

    const boost::ssub_match first = match[1];
    const boost::ssub_match second = match[2];

    EXPECT_EQ(first.compare(first), 0);
    EXPECT_LT(second.compare(first), 0);
    EXPECT_GT(first.compare(second), 0);

    EXPECT_TRUE(first == first);
    EXPECT_TRUE(first != second);
    EXPECT_TRUE(first >= second);
    EXPECT_TRUE(second <= first);
    EXPECT_TRUE(first > second);
    EXPECT_TRUE(second < first);
}

TEST(SubMatch, SubMatchCToOutputStream) {
    const std::string input_str = "id=123";
    const boost::regex pattern(R"(id=(\d+))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    std::ostringstream output;
    output << match[1];

    EXPECT_EQ("123", output.str());
}