#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>
#include <vector>

// #### Capture Groups ####

TEST(RegexGroups, SimpleCaptureGroup) {
    boost::regex pattern(R"((\d+))");
    std::string s = "abc 123 edf";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m.position(std::size_t{0}), 4);
    EXPECT_EQ(m.length(std::size_t{0}), 3);
    EXPECT_EQ(m.prefix().str(), "abc ");
    EXPECT_EQ(m.suffix().str(), " edf");
    EXPECT_EQ(m[0].str(), "123");
    EXPECT_EQ(m[1].str(), "123");
    EXPECT_EQ(m.size(), 2u);
}

TEST(RegexGroups, MultipleCaptureGroups) {
    boost::regex pattern(R"((\w+)@(\w+)\.(\w+))");
    std::string s = "user@example.com";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[1].str(), "user");
    EXPECT_EQ(m[2].str(), "example");
    EXPECT_EQ(m[3].str(), "com");
}

TEST(RegexGroups, NestedCaptureGroups) {
    boost::regex pattern(R"(((a)(b))c)");
    std::string s = "abc";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[0].str(), "abc");
    EXPECT_EQ(m[1].str(), "ab");
    EXPECT_EQ(m[2].str(), "a");
    EXPECT_EQ(m[3].str(), "b");
}

TEST(RegexGroups, NamedGroups) {
    boost::regex pattern(R"((?<day>\d{2})\.(?<month>\d{2})\.(?<year>\d{4}))");
    boost::regex patternAlt(R"((?'day'\d{2})\.(?'month'\d{2})\.(?'year'\d{4}))");
    std::string s = "01.01.2001";
    
    auto AssertNamedGroups = [&s](const boost::regex& pattern) {
        boost::smatch m;
        ASSERT_TRUE(boost::regex_match(s, m, pattern));
        EXPECT_EQ(m["day"].str(), "01");
        EXPECT_EQ(m["month"].str(), "01");
        EXPECT_EQ(m["year"].str(), "2001");
    };

    AssertNamedGroups(pattern);
    AssertNamedGroups(patternAlt);
}

TEST(RegexGroups, NamedGroupsAccessibleByNameAndIndex) {
    boost::regex pattern(R"((?<first>\w+)\s+(?<second>\w+))");
    std::string s = "hello world";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[1].str(), "hello");
    EXPECT_EQ(m[2].str(), "world");
    EXPECT_EQ(m["first"].str(),  m[1].str());
    EXPECT_EQ(m["second"].str(), m[2].str());
}

TEST(RegexGroups, InvalidNamedGroupThrowsRegexError) {
    try {
        boost::regex pattern(R"((?<word>\w+)");
        FAIL() << "Expected boost::regex_error for invalid named group syntax";
    } catch (const boost::regex_error& e) {
        EXPECT_EQ(e.code(), boost::regex_constants::error_paren);
    }
}

// #### Non-capturing Groups ####

TEST(RegexGroups, NonCapturingGroup) {
    boost::regex pattern(R"((?:foo|test)(\d+))");
    std::string s = "foo420";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[0].str(), "foo420");
    EXPECT_EQ(m[1].str(), "420");
    EXPECT_EQ(m.size(), 2u);
}

TEST(RegexGroups, NonCapturingVsCapturing) {
    boost::regex capturing(R"((foo|test)(\d+))");
    boost::regex nonCapturing(R"((?:foo|test)(\d+))");
    std::string s = "test7";

    boost::smatch m1;
    ASSERT_TRUE(boost::regex_match(s, m1, capturing));
    EXPECT_EQ(m1.size(), 3u); // whole + 2 groups

    boost::smatch m2;
    ASSERT_TRUE(boost::regex_match(s, m2, nonCapturing));
    EXPECT_EQ(m2.size(), 2u); // whole + 1 group
}

// #### Optional Groups ####

TEST(RegexGroups, OptionalGroupMatched) {
    boost::regex pattern(R"((https?)://(\w+)(:(\d+))?)");
    std::string s = "http://localhost:8080";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[1].str(), "http");
    EXPECT_EQ(m[2].str(), "localhost");
    EXPECT_TRUE(m[3].matched);
    EXPECT_EQ(m[4].str(), "8080");
}

TEST(RegexGroups, OptionalGroupNotMatched) {
    boost::regex pattern(R"((https?)://(\w+)(:(\d+))?)");
    std::string s = "http://localhost";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    EXPECT_EQ(m[1].str(), "http");
    EXPECT_EQ(m[2].str(), "localhost");
    // Optional group not found
    EXPECT_FALSE(m[3].matched);
    EXPECT_FALSE(m[4].matched);
    EXPECT_EQ(m[4].str(), "");
}

// #### Backreferences ####

TEST(RegexGroups, BackreferenceMatching) {
    boost::regex pattern(R"(^([^,]+),\1$)");
    boost::smatch m;
    const auto s = std::string("hello,hello");
    ASSERT_TRUE(boost::regex_match(s, m, pattern));
    ASSERT_EQ(m.size(), 2u);
    EXPECT_EQ(m[0].str(), "hello,hello");
    EXPECT_EQ(m[1].str(), "hello");

    EXPECT_FALSE(boost::regex_match(std::string("hello,world"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("hello,hello,hello"), pattern));
}

TEST(RegexGroups, BackreferenceMatchingCaseInsensitive) {
    boost::regex pattern(R"(<([A-Z][A-Z0-9]*)\b[^>]*>.*?</\1>)", boost::regex::icase);
    EXPECT_TRUE(boost::regex_match(std::string("<p> This is a paragraph </p>"), pattern));
    EXPECT_TRUE(boost::regex_match(std::string("<H1> Tags are case insensitive </h1>"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("<div> Mismatched tag </p>"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("<div> Improper closing tag <div>"), pattern));
}

TEST(RegexGroups, BackreferenceRepeatedWord) {
    boost::regex pattern(R"(\b(\w+)\s+\1\b)");
    boost::smatch m;
    const auto s = std::string("goodbye vs bye bye");
    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    ASSERT_EQ(m.size(), 2u);
    EXPECT_EQ(m[0].str(), "bye bye");
    EXPECT_EQ(m[1].str(), "bye");

    EXPECT_FALSE(boost::regex_search(std::string("all unique words"), pattern));
}

TEST(RegexGroups, BackreferenceMultipleMatching) {
    boost::regex pattern(R"((\w)(\d)\1\2)");
    boost::smatch m;
    const auto s = std::string("a1a1");
    ASSERT_TRUE(boost::regex_search(s, m, pattern));
    EXPECT_EQ(m[1].str(), "a");
    EXPECT_EQ(m[2].str(), "1");

    EXPECT_FALSE(boost::regex_match(std::string("a1a2"), pattern));
    EXPECT_FALSE(boost::regex_match(std::string("a1b1"), pattern));
}

TEST(RegexGroups, RepeatedCaptureGroupStoresLastIteration) {
    boost::regex pattern(R"(((\d)+))");
    boost::smatch m;
    const auto s = std::string("1234");
    ASSERT_TRUE(boost::regex_match(s, m, pattern));

    EXPECT_EQ(m[0].str(), "1234");
    EXPECT_EQ(m[1].str(), "1234");
    EXPECT_EQ(m[2].str(), "4");
}

TEST(RegexGroups, NamedBackreference) {
    const auto validStr = std::string("<p> This is a paragraph </p>");
    const auto invalidStr = std::string("<div> Mismatched tag </p>");

    boost::regex pattern(R"(<(?<tag>[A-Z][A-Z0-9]*)\b[^>]*>.*?</\k<tag>>)", boost::regex::icase);
    EXPECT_TRUE(boost::regex_match(validStr, pattern));
    EXPECT_FALSE(boost::regex_match(invalidStr, pattern));
 
    boost::regex patternAlt(R"(<(?<tag>[A-Z][A-Z0-9]*)\b[^>]*>.*?</\g{tag}>)", boost::regex::icase);
    EXPECT_TRUE(boost::regex_match(validStr, patternAlt));
    EXPECT_FALSE(boost::regex_match(invalidStr, patternAlt));

}

// #### Replace using groups ####

TEST(RegexGroups, ReplaceWithBackreference) {
   boost::regex pattern(R"((\w+)(\d)(\d))");
   std::string result = boost::regex_replace(
        std::string("abc12"), 
        pattern, 
        std::string("<$1$2><$1$3>"));
    EXPECT_EQ(result, "<abc1><abc2>");
}

TEST(RegexGroups, ReplaceWithBackreferenceUnchanged) {
   boost::regex pattern(R"((\w+)(\d)(\d))");
   std::string result = boost::regex_replace(
        std::string("abc"), 
        pattern, 
        std::string("<$1$2><$1$3>"));
    EXPECT_EQ(result, "abc");
}

TEST(RegexGroups, ReplaceWithNamedBackreference) {
    boost::regex pattern(R"((?<word>\w+)(?<digit1>\d)(?<digit2>\d))");
    std::string result = boost::regex_replace(
        std::string("abc12"), 
        pattern, 
        std::string(R"(<$+{word}$+{digit1}><$+{word}$+{digit2}>)"));
    EXPECT_EQ(result, "<abc1><abc2>");
}

TEST(RegexGroups, ReplaceWithOptionalGroup) {
    boost::regex pattern(R"((https?)://(\w+)(:(\d+))?)");
    std::string result1 = boost::regex_replace(
        std::string("http://localhost"),
        pattern,
        std::string("scheme=$1 host=$2 port=$4"));
    EXPECT_EQ(result1, "scheme=http host=localhost port=");

    std::string result2 = boost::regex_replace(
        std::string("https://localhost:8080"),
        pattern,
        std::string("scheme=$1 host=$2 port=$4"));
    EXPECT_EQ(result2, "scheme=https host=localhost port=8080");
}

// #### Group iteration ####

TEST(RegexGroups, IterateOverAllGroupsInMatch) {
    boost::regex pattern(R"((\d{1,2}).(\d{1,2}).(\d{2,4}))");
    std::string s = "01.01.2001";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, pattern));

    std::vector<std::string> groups;
    for (size_t i = 1; i < m.size(); ++i)
        groups.push_back(m[i].str());

    ASSERT_EQ(groups.size(), 3u);
    EXPECT_EQ(groups[0], "01");
    EXPECT_EQ(groups[1], "01");
    EXPECT_EQ(groups[2], "2001");
}

