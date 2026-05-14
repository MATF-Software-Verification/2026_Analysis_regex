#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <tuple>

// #### Iterators ####

TEST(RegexIterator, FindAllMatches) {
    boost::regex pattern(R"(\b\w+\b)");
    std::string s = "This is a test.";
    std::vector<std::string> expected = {"This", "is", "a", "test"};
    
    std::vector<std::string> actual;
    boost::sregex_iterator it(s.begin(), s.end(), pattern);
    boost::sregex_iterator end;
    std::for_each(it, end, [&actual](const boost::smatch& m) {
        actual.push_back(m.str());
    });
    
    ASSERT_EQ(actual.size(), expected.size());
    EXPECT_EQ(actual, expected);
}

TEST(RegexIterator, NoMatchesIterator) {
    boost::regex pattern(R"(\d+)");
    std::string s = "No digits here";
    
    boost::sregex_iterator it(s.begin(), s.end(), pattern);
    boost::sregex_iterator end;
    
    EXPECT_EQ(it, end); 
}

TEST(RegexIterator, CaptureGroupsWithIterator) {
    boost::regex pattern(R"((\w+)=(\d+))");
    std::string s = "x=1 y=2 z=3";
    boost::sregex_iterator it(s.begin(), s.end(), pattern);
    boost::sregex_iterator end;

    std::vector<std::tuple<std::string, std::string>> expected = {
        {"x", "1"},
        {"y", "2"},
        {"z", "3"}
    };
    std::vector<std::tuple<std::string, std::string>> actual;
    std::for_each(it, end, [&actual](const boost::smatch& m) { // same as const boost::match_results<std::string::const_iterator>
        actual.emplace_back(m[1].str(), m[2].str());
    });

    ASSERT_EQ(actual.size(), expected.size());
    EXPECT_EQ(actual[0], expected[0]);
    EXPECT_EQ(actual[1], expected[1]);  
    EXPECT_EQ(actual[2], expected[2]);
}

TEST(RegexIterator, OverlappingMatches) {
    boost::regex pattern(R"(\d{2})");
    std::string s = "123";
    boost::sregex_iterator it(s.begin(), s.end(), pattern);
    boost::sregex_iterator end;

    std::vector<std::string> matches;
    std::for_each(it, end, [&matches](const boost::smatch& m) {
        matches.push_back(m.str());
    });

    ASSERT_EQ(matches.size(), 1);
    EXPECT_EQ(matches[0], "12");
}

// #### Token iterator ####     

TEST(RegexTokenIterator, SplitString) {
    boost::regex pattern(R"(,\s*)");
    std::string s = "one, two, three,four";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, -1); // if submatch is -1, then enumerates all the text sequences that did not match the expression regex
    boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    });

    ASSERT_EQ(tokens.size(), 4);
    EXPECT_EQ(tokens[0], "one");
    EXPECT_EQ(tokens[3], "four");
} 

TEST(RegexTokenIterator, ExtractWholeSubmatch) {
    boost::regex pattern(R"((\d{2})\.(\d{2})\.(\d{4}))");
    std::string s = "01.01.2001, 02.02.2002";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, 0);
    boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    });

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "01.01.2001");
    EXPECT_EQ(tokens[1], "02.02.2002");

} 

TEST(RegexTokenIterator, ExtractCaptureGroupTokens) {
    boost::regex pattern(R"((\d{2})\.(\d{2})\.(\d{4}))");
    std::string s = "01.01.2001";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, {1, 2, 3}); // if submatch is a positive integer n, then enumerates the text that matches the nth submatch of the expression regex
    boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    });

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "01");
    EXPECT_EQ(tokens[1], "01");
    EXPECT_EQ(tokens[2], "2001");
}

TEST(RegexTokenIterator, EmptyTokensBetweenSeparators) {
    boost::regex pattern(R"(,)");
    std::string s = "a,,b,";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, -1);
    boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    }); 

    ASSERT_EQ(tokens.size(), 3);
    EXPECT_EQ(tokens[0], "a");
    EXPECT_EQ(tokens[1], ""); 
    EXPECT_EQ(tokens[2], "b");
}



// #### Iterator Cases ####

TEST(RegexIterator, ExtractQuotedStringsWithBackreference) {
    const boost::regex pattern(R"((['"])(.*?)\1)");
    const std::string s = R"("hello" and 'world')";

    boost::sregex_iterator it(s.begin(), s.end(), pattern);
    boost::sregex_iterator end;

    std::vector<std::string> quotes;
    std::vector<std::string> contents;

    for (; it != end; ++it) {
        quotes.push_back((*it)[1].str());
        contents.push_back((*it)[2].str());
    }

    ASSERT_EQ(quotes.size(), 2u);
    ASSERT_EQ(contents.size(), 2u);
    EXPECT_EQ(quotes[0], "\"");
    EXPECT_EQ(contents[0], "hello");
    EXPECT_EQ(quotes[1], "'");
    EXPECT_EQ(contents[1], "world");
}

TEST(RegexTokenIterator, KeepSeparatorsAndTokens) {
    const boost::regex pattern(R"(,\s*)");
    const std::string s = "one, two,three";

    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, {-1, 0});
    boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    for (; it != end; ++it) {
        tokens.push_back(it->str());
    }

    ASSERT_EQ(tokens.size(), 5u);
    EXPECT_EQ(tokens[0], "one");
    EXPECT_EQ(tokens[1], ", ");
    EXPECT_EQ(tokens[2], "two");
    EXPECT_EQ(tokens[3], ",");
    EXPECT_EQ(tokens[4], "three");
}


