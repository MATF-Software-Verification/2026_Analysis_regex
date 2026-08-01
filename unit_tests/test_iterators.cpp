#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>
#include <string>
#include <vector>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <cstring>

// #### Iterators ####

TEST(RegexIterator, DefaultConstruction) {
    const boost::sregex_iterator first;
    const boost::sregex_iterator second;

    EXPECT_EQ(first, second);
}

TEST(RegexIterator, CopyConstructedIterator) {
    const std::string input_str = "id=123 foo=42";
    const boost::regex pattern(R"((\w+)=(\d+))");

    const boost::sregex_iterator original(input_str.begin(), input_str.end(), pattern);
    const boost::sregex_iterator copied(original);

    ASSERT_NE(original, boost::sregex_iterator());
    ASSERT_NE(copied, boost::sregex_iterator());

    EXPECT_EQ((*original)[0].str(), (*copied)[0].str());
    EXPECT_EQ((*original)[1].str(), (*copied)[1].str());
    EXPECT_EQ((*original)[2].str(), (*copied)[2].str());

}

TEST(RegexIterator, AssignedIterator) {
    const std::string input_str = "id=123 foo=42";
    const boost::regex pattern(R"((\w+)=(\d+))");

    const boost::sregex_iterator original(input_str.begin(), input_str.end(), pattern);
    boost::sregex_iterator assigned;

    assigned = original;

    ASSERT_NE(original, boost::sregex_iterator());
    ASSERT_NE(assigned, boost::sregex_iterator());

    EXPECT_EQ((*original)[0].str(), (*assigned)[0].str());
    EXPECT_EQ((*original)[1].str(), (*assigned)[1].str());
    EXPECT_EQ((*original)[2].str(), (*assigned)[2].str());

}


TEST(RegexIterator, IncrementMovesToNextMatch) {
    const std::string input_str = "id=123 foo=42";
    const boost::regex pattern(R"((\w+)=(\d+))");

    boost::sregex_iterator iterator(input_str.begin(), input_str.end(), pattern);
    const boost::sregex_iterator end;

    ASSERT_NE(iterator, end);

    EXPECT_EQ("id=123", (*iterator)[0].str());

    ++iterator;

    ASSERT_NE(iterator, end);
    EXPECT_EQ("foo=42", (*iterator)[0].str());

    iterator++;

    EXPECT_EQ(iterator, end);
}

TEST(RegexIterator, PostIncrementReturnsPreviousMatch) {
    const std::string input_str = "id=123 foo=42";
    const boost::regex pattern(R"((\w+)=(\d+))");

    boost::sregex_iterator iterator(input_str.begin(), input_str.end(), pattern);
  
    const boost::smatch previous = *iterator++;

    ASSERT_NE(iterator, boost::sregex_iterator());

    EXPECT_EQ("id=123", previous[0].str());
    EXPECT_EQ("foo=42", (*iterator)[0].str());
}

TEST(RegexIterator, MatchNotNullSkipsEmptyMatches)
{
    const std::string input_str = "abc";
    const boost::regex pattern(R"(z*)");

    const boost::sregex_iterator iterator(input_str.begin(), input_str.end(), pattern);
    const boost::sregex_iterator not_null_iterator(
        input_str.begin(),
        input_str.end(),
        pattern,
        boost::regex_constants::match_not_null);

    ASSERT_NE(iterator, boost::sregex_iterator());
    EXPECT_EQ("", iterator->str());

    EXPECT_EQ(not_null_iterator, boost::sregex_iterator());
}



TEST(RegexIterator, CRegexIteratorOnConstCharPointerRange) {
    boost::regex pattern(R"(\b\w+\b)");
    const char* s = "This is a test.";
    std::vector<std::string> expected = {"This", "is", "a", "test"};
    
    std::vector<std::string> actual;
    boost::cregex_iterator it(s, s + std::strlen(s), pattern);
    boost::cregex_iterator end;
    std::for_each(it, end, [&actual](const boost::cmatch& m) {
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
    std::for_each(it, end, [&actual](const boost::smatch& m) {
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

TEST(RegexIterator, SourceStringMustOutliveIterator) {
    #ifndef __SANITIZE_ADDRESS__
        GTEST_SKIP() << "Testing undefined behaviour, please enable ASan.";
    #endif
    
    EXPECT_DEATH(
        ([](){
            boost::regex pattern(R"(\d+)");
            boost::sregex_iterator it, end;
            {
                std::string s = "1 22 333";
                it = boost::sregex_iterator(s.begin(), s.end(), pattern);
                // s destroyed here
            }
            for (; it != end; ++it)
                (void)(*it)[0].str();
    })(), "");
}

// #### Token iterator ####     

TEST(RegexTokenIterator, DefaultConstruction) {
    const boost::sregex_token_iterator first;
    const boost::sregex_token_iterator second;

    EXPECT_EQ(first, second);
}

TEST(RegexTokenIterator, CopyConstructedIterator) {
    const std::string input_str = "one,two,three";
    const boost::regex pattern(R"(,)");

    const boost::sregex_token_iterator original(input_str.begin(), input_str.end(), pattern, -1);
    const boost::sregex_token_iterator copied(original);

    ASSERT_NE(original, boost::sregex_token_iterator());
    ASSERT_NE(copied, boost::sregex_token_iterator());

    EXPECT_EQ((*original).str(), (*copied).str());
    EXPECT_EQ("one", (*copied).str());
}

TEST(RegexTokenIterator, AssignedIterator) {
    const std::string input_str = "one,two,three";
    const boost::regex pattern(R"(,)");

    const boost::sregex_token_iterator original(input_str.begin(), input_str.end(), pattern, -1);
    boost::sregex_token_iterator assigned;

    assigned = original;

    ASSERT_NE(original, boost::sregex_token_iterator());
    ASSERT_NE(assigned, boost::sregex_token_iterator());

    EXPECT_EQ((*original).str(), (*assigned).str());
    EXPECT_EQ("one", (*assigned).str());
}


TEST(RegexTokenIterator, IncrementMovesToNextToken) {
    const std::string input_str = "one,two";
    const boost::regex pattern(R"(,)");

    boost::sregex_token_iterator iterator(input_str.begin(), input_str.end(), pattern, -1);
    const boost::sregex_token_iterator end;

    ASSERT_NE(iterator, end);

    EXPECT_EQ("one", iterator->str());

    ++iterator;

    ASSERT_NE(iterator, end);
    EXPECT_EQ("two", iterator->str());

    iterator++;

    EXPECT_EQ(iterator, end);
}

TEST(RegexTokenIterator, PostIncrementReturnsPreviousToken) {
    const std::string input_str = "one,two";
    const boost::regex pattern(R"(,)");

    boost::sregex_token_iterator iterator(input_str.begin(), input_str.end(), pattern, -1);
  
    const boost::ssub_match previous = *iterator++;

    ASSERT_NE(iterator, boost::sregex_token_iterator());

    EXPECT_EQ("one", previous.str());
    EXPECT_EQ("two", iterator->str());
}

TEST(RegexTokenIterator, SplitNoMatchReturnsWholeInput) {
    const boost::regex pattern(R"(,)");
    const std::string input_str = "one two three";

    boost::sregex_token_iterator iterator(input_str.begin(), input_str.end(), pattern, -1); 
    const boost::sregex_token_iterator end;

    ASSERT_NE(iterator, end);

    EXPECT_EQ(input_str, iterator->str());

    ++iterator;

    EXPECT_EQ(iterator, end);

} 

TEST(RegexTokenIterator, ExtractWholeSubmatch) {
    const boost::regex pattern(R"((\d{2})\.(\d{2})\.(\d{4}))");
    const std::string s = "01.01.2001, 02.02.2002";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, 0);
    const boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    });

    ASSERT_EQ(tokens.size(), 2);
    EXPECT_EQ(tokens[0], "01.01.2001");
    EXPECT_EQ(tokens[1], "02.02.2002");

} 

TEST(RegexTokenIterator, ExtractCaptureGroupTokens) {
    const boost::regex pattern(R"((\d{2})\.(\d{2})\.(\d{4}))");
    const std::string s = "01.01.2001, 02.02.2002";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, {1, 2, 3});
    const boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    });

    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ(tokens[0], "01");
    EXPECT_EQ(tokens[1], "01");
    EXPECT_EQ(tokens[2], "2001");

    EXPECT_EQ(tokens[3], "02");
    EXPECT_EQ(tokens[4], "02");
    EXPECT_EQ(tokens[5], "2002");
}

TEST(RegexTokenIterator, EmptyTokensBetweenSeparators) {
    const boost::regex pattern(R"(,)");
    const std::string s = "a,,b,";
    boost::sregex_token_iterator it(s.begin(), s.end(), pattern, -1);
    const boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(it, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    }); 

    ASSERT_EQ(tokens.size(), 3); 
    EXPECT_EQ(tokens[0], "a");
    EXPECT_EQ(tokens[1], ""); 
    EXPECT_EQ(tokens[2], "b");
}


TEST(RegexTokenIterator, MixedSplitAndCaptureGroupTokens)
{
    const std::string input_str = "x=1;y=2";
    const boost::regex pattern(R"((\w+)=(\d+))");

    boost::sregex_token_iterator iterator(input_str.begin(), input_str.end(), pattern, {-1, 1, 2});
    const boost::sregex_token_iterator end;

    std::vector<std::string> tokens;
    std::for_each(iterator, end, [&tokens](const std::string& m) {
        tokens.push_back(m);
    }); 

    ASSERT_EQ(tokens.size(), 6);

    EXPECT_EQ("", tokens[0]);
    EXPECT_EQ("x", tokens[1]);
    EXPECT_EQ("1", tokens[2]);

    EXPECT_EQ(";", tokens[3]);
    EXPECT_EQ("y", tokens[4]);
    EXPECT_EQ("2", tokens[5]);
}

