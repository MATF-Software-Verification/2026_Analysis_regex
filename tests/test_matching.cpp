#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>


// #### Regex Match ####

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
    EXPECT_TRUE(boost::regex_match("123", pattern));
    EXPECT_FALSE(boost::regex_match("123abc", pattern));
}

TEST(RegexMatch, PopulateMatchResults) {
    const std::string input_str = "id=123";
    const boost::regex pattern(R"((\w+)=(\d+))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_match(input_str, match, pattern));

    ASSERT_EQ(3u, match.size());
    EXPECT_EQ("id=123", match[0].str());
    EXPECT_EQ("id", match[1].str());
    EXPECT_EQ("123", match[2].str());
    EXPECT_EQ(0, match.position(0uz));
    EXPECT_EQ(6, match.length(0));
    EXPECT_TRUE(match.prefix().str().empty());
    EXPECT_TRUE(match.suffix().str().empty());
    EXPECT_TRUE(match.prefix().str().empty());
    EXPECT_TRUE(match.suffix().str().empty());
    EXPECT_FALSE(match.prefix().matched);
    EXPECT_FALSE(match.suffix().matched);
}


TEST(RegexMatch, RegexMatchPostconditionsForFullMatch) {
    const std::string input_str = "prefix id=123 suffix";
    const auto first = input_str.begin() + 7;
    const auto last = input_str.begin() + 13;
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::match_results<std::string::const_iterator> match;

    ASSERT_TRUE(boost::regex_match(first, last, match, pattern));

    EXPECT_EQ(match.size(), 1 + pattern.mark_count());
    EXPECT_FALSE(match.empty());

    EXPECT_TRUE(match.prefix().str().empty());
    EXPECT_TRUE(match.suffix().str().empty());
    EXPECT_FALSE(match.prefix().matched);
    EXPECT_FALSE(match.suffix().matched);
    EXPECT_EQ(match.prefix().first, first);
    EXPECT_EQ(match.prefix().second, first);
    EXPECT_EQ(match.suffix().first, last);
    EXPECT_EQ(match.suffix().second, last);

    EXPECT_TRUE(match[0].matched);
    EXPECT_EQ(match[0].first, first);
    EXPECT_EQ(match[0].second, last);
    for(int i = 1; i < match.size(); ++i) {
        EXPECT_TRUE(match[i].matched);
        EXPECT_EQ(match[i].first, first + match.position(i));
        EXPECT_EQ(match[i].second, first + match.position(i) + match.length(i));
    }
}

TEST(RegexMatch, RegexMatchPostconditionsForPartialMatch) {
    const std::string input_str = "123-45678";
    const auto first = input_str.begin();
    const auto last = input_str.end();
    const boost::regex pattern(R"((\d{3,4})[- ]?(\d{4})[- ]?(\d{4})[- ]?(\d{4}))");
    boost::match_results<std::string::const_iterator> match;

    ASSERT_TRUE(boost::regex_match(first, last, match, pattern, boost::regex_constants::match_partial));

    EXPECT_EQ(match.position(0uz), 0);
    EXPECT_EQ(match.size(), 1u + pattern.mark_count());
    EXPECT_FALSE(match.empty());

    EXPECT_TRUE(match.prefix().str().empty());
    EXPECT_TRUE(match.suffix().str().empty());
    EXPECT_FALSE(match.prefix().matched);
    EXPECT_FALSE(match.suffix().matched);
    EXPECT_EQ(match.prefix().first, first);
    EXPECT_EQ(match.prefix().second, first);
    EXPECT_EQ(match.suffix().first, last);
    EXPECT_EQ(match.suffix().second, last);

    EXPECT_FALSE(match[0].matched);
    EXPECT_EQ(match[0].first, first);
    EXPECT_EQ(match[0].second, last);
    for(int i = 1; i < match.size(); ++i) {
        if(match[i].matched) {
            EXPECT_EQ(match[i].first, first + match.position(i));
            EXPECT_EQ(match[i].second, first + match.position(i) + match.length(i));
        } 
        else {
            EXPECT_EQ(match[i].first, last);
            EXPECT_EQ(match[i].second, last);
        }
    }
}

TEST(RegexMatch, CompareConstructionOverloadsOnMatchedInput) {
    const std::string input_str = "123-1234";
    boost::regex pattern(R"(\d{3}-\d{4})"); 

    boost::match_results<std::string::const_iterator> match_iter;
    EXPECT_TRUE(boost::regex_match(input_str.begin(), input_str.end(), match_iter, pattern));
    EXPECT_TRUE(boost::regex_match(input_str.begin(), input_str.end(), pattern));

    boost::cmatch match_cstr;
    EXPECT_TRUE(boost::regex_match(input_str.c_str(), pattern));
    EXPECT_TRUE(boost::regex_match(input_str.c_str(), match_cstr, pattern));

    boost::smatch match_str;
    EXPECT_TRUE(boost::regex_match(input_str, pattern));
    EXPECT_TRUE(boost::regex_match(input_str, match_str, pattern));
}

TEST(RegexMatch, CompareConstructionOverloadsOnUnmatchedInput) {
    const std::string input_str = "prefix 123-1234 suffix";
    const boost::regex pattern(R"((\d{3})-(\d{4}))");

    boost::match_results<std::string::const_iterator> match_iter;
    EXPECT_FALSE(boost::regex_match(input_str.begin(), input_str.end(), pattern));
    EXPECT_FALSE(boost::regex_match(input_str.begin(), input_str.end(), match_iter, pattern));

    boost::cmatch match_cstr;
    EXPECT_FALSE(boost::regex_match(input_str.c_str(), match_cstr, pattern));
    EXPECT_FALSE(boost::regex_match(input_str.c_str(), pattern));

    boost::smatch match_str;
    EXPECT_FALSE(boost::regex_match(input_str, pattern));
    EXPECT_FALSE(boost::regex_match(input_str, match_str, pattern));
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

TEST(RegexSearch, CompareConstructionOverloadsMatchExists) {
    const std::string input_str = "prefix 123-1234 suffix";
    boost::regex pattern(R"(\d{3}-\d{4})"); 

    boost::match_results<std::string::const_iterator> match_iter;
    EXPECT_TRUE(boost::regex_search(input_str.begin(), input_str.end(), match_iter, pattern));
    EXPECT_TRUE(boost::regex_search(input_str.begin(), input_str.end(), pattern));

    boost::cmatch match_cstr;
    EXPECT_TRUE(boost::regex_search(input_str.c_str(), pattern));
    EXPECT_TRUE(boost::regex_search(input_str.c_str(), match_cstr, pattern));

    boost::smatch match_str;
    EXPECT_TRUE(boost::regex_search(input_str, pattern));
    EXPECT_TRUE(boost::regex_search(input_str, match_str, pattern));
}


TEST(RegexSearch, CompareConstionOverloadsNoMatchExists) {
    const std::string input_str = "prefix abc suffix";
    const boost::regex pattern(R"((\d{3})-(\d{4}))");

    boost::match_results<std::string::const_iterator> match_iter;
    EXPECT_FALSE(boost::regex_search(input_str.begin(), input_str.end(), pattern));
    EXPECT_FALSE(boost::regex_search(input_str.begin(), input_str.end(), match_iter, pattern));

    boost::cmatch match_cstr;
    EXPECT_FALSE(boost::regex_search(input_str.c_str(), match_cstr, pattern));
    EXPECT_FALSE(boost::regex_search(input_str.c_str(), pattern));

    boost::smatch match_str;
    EXPECT_FALSE(boost::regex_search(input_str, pattern));
    EXPECT_FALSE(boost::regex_search(input_str, match_str, pattern));
}

TEST(RegexSearch, RegexSearchPostconditionsForFullMatch) {
    const std::string input_str = "prefix id=123 suffix";
    const auto first = input_str.begin();
    const auto last = input_str.end();
    const boost::regex pattern(R"((\w+)=(\d+))");
    boost::match_results<std::string::const_iterator> match;

    ASSERT_TRUE(boost::regex_search(first, last, match, pattern));

    EXPECT_EQ(match.size(), 1uz + pattern.mark_count());
    EXPECT_FALSE(match.empty());

    EXPECT_EQ(match.prefix().matched, match.prefix().first != match.prefix().second);
    EXPECT_EQ(match.suffix().matched,  match.suffix().first != match.suffix().second);
    EXPECT_EQ(match.prefix().first, first);
    EXPECT_EQ(match.prefix().second, match[0].first);
    EXPECT_EQ(match.suffix().first, match[0].second);
    EXPECT_EQ(match.suffix().second, last);

    EXPECT_TRUE(match[0].matched);
    EXPECT_EQ(match[0].first, first + 7);
    EXPECT_EQ(match[0].second, first + 13);
    for(int i = 1; i < match.size(); ++i) {
        EXPECT_TRUE(match[i].matched);
        EXPECT_EQ(match[i].first, first + match.position(i));
        EXPECT_EQ(match[i].second, first + match.position(i) + match.length(i));
    }
}

TEST(RegexSearch, RegexSearchPostconditionsForPartialMatch) {
    const std::string input_str = "123-45678";
    const auto first = input_str.begin();
    const auto last = input_str.end();
    const boost::regex pattern(R"((\d{3,4})[- ]?(\d{4})[- ]?(\d{4})[- ]?(\d{4}))");
    boost::match_results<std::string::const_iterator> match;

    ASSERT_TRUE(boost::regex_search(first, last, match, pattern, boost::regex_constants::match_partial));

    EXPECT_EQ(match.size(), 1u + pattern.mark_count());
    EXPECT_FALSE(match.empty());

    EXPECT_EQ(match.prefix().matched, match.prefix().first != match.prefix().second);
    EXPECT_EQ(match.suffix().matched,  match.suffix().first != match.suffix().second);
    EXPECT_EQ(match.prefix().first, first);
    EXPECT_EQ(match.prefix().second, match[0].first);
    EXPECT_EQ(match.suffix().first, match[0].second);
    EXPECT_EQ(match.suffix().second, last);

    EXPECT_FALSE(match[0].matched);
    EXPECT_EQ(match[0].first, first);
    EXPECT_EQ(match[0].second, last);
    for(int i = 1; i < match.size(); ++i) {
        if(match[i].matched) {
            EXPECT_EQ(match[i].first, first + match.position(i));
            EXPECT_EQ(match[i].second, first + match.position(i) + match.length(i));
        } 
        else {
            EXPECT_EQ(match[i].first, last);
            EXPECT_EQ(match[i].second, last);
        }
    }
}

TEST(RegexSearch, FindsFirstOccurrenceOnly)
{
    const std::string input_str = "id=100 id=200 id=300";
    const boost::regex pattern(R"(id=(\d+))");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    ASSERT_EQ(match.size(), 2uz);
    EXPECT_EQ(match[0].str(), "id=100");
    EXPECT_EQ(match[1].str(), "100");
    EXPECT_EQ(match.position(0uz), 0);
}

TEST(RegexSearch, ReportsUnmatchedOptionalSubexpression) {
    const std::string input_str = "abc";
    const boost::regex pattern(R"(abc(\d+)?)");

    boost::smatch match;

    ASSERT_TRUE(boost::regex_search(input_str, match, pattern));

    ASSERT_EQ(2u, match.size());
    EXPECT_EQ("abc", match[0].str());
    EXPECT_FALSE(match[1].matched);
    EXPECT_EQ("", match[1].str());
    EXPECT_EQ(0, match.length(1));
}



