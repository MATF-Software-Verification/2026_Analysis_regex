#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>
#include <optional>

namespace {

    void ExpectRegexError(const std::string& pattern, boost::regex_constants::error_type expectedCode, std::optional<boost::regex::flag_type> flags = std::nullopt) {
        try {
            boost::regex re(pattern, flags.value_or(boost::regex::perl));
            FAIL() << "Expected boost::regex_error for pattern: " << pattern;
        } catch (const boost::regex_error& error) {
            EXPECT_EQ(error.code(), expectedCode);
            EXPECT_FALSE(std::string(error.what()).empty());
        }  
    }

} //namespace

TEST(RegexError, ParanthesisError) {
    ExpectRegexError("(", boost::regex_constants::error_paren);
    ExpectRegexError(")", boost::regex_constants::error_paren);
    ExpectRegexError("(abc", boost::regex_constants::error_paren);
    ExpectRegexError("abc)", boost::regex_constants::error_paren);
}

TEST(RegexError, BracketError) {
    // [ is a special character im Perl, POSIX-Basic and POSIX-extended regex syntaxes 
    // but not ]
    ExpectRegexError("[", boost::regex_constants::error_brack);
    ExpectRegexError("[abc", boost::regex_constants::error_brack);
    ExpectRegexError("[abc[", boost::regex_constants::error_brack);
    ExpectRegexError(R"([abc\])", boost::regex_constants::error_brack);
}

TEST(RegexError, BraceErrorExtended) {
    ExpectRegexError("a{}", boost::regex_constants::error_brace, boost::regex::extended);
    ExpectRegexError("a{1", boost::regex_constants::error_brace, boost::regex::extended);
    ExpectRegexError("a3}", boost::regex_constants::error_brace, boost::regex::extended);
    ExpectRegexError("a{1,3{", boost::regex_constants::error_brace, boost::regex::extended);
    ExpectRegexError(R"(a{b}c)", boost::regex_constants::error_brace, boost::regex::extended);
}

TEST(RegexError, NoBraceErrorPerl) {
    EXPECT_NO_THROW(boost::regex("a{}"));
    EXPECT_NO_THROW(boost::regex("a{1"));
    EXPECT_NO_THROW(boost::regex("a3}"));
    EXPECT_NO_THROW(boost::regex("a{1,3{"));
    EXPECT_NO_THROW(boost::regex("a{b}c"));
}

TEST(RegexError, BadBraceError) {
    ExpectRegexError("a{3,1}", boost::regex_constants::error_badbrace);
}

// TEST(RegexError, BadRepeatError) {
//     ExpectRegexError("*", boost::regex_constants::error_badrepeat);
//     ExpectRegexError("+", boost::regex_constants::error_badrepeat);
//     ExpectRegexError("?", boost::regex_constants::error_badrepeat);
//     ExpectRegexError("a*+", boost::regex_constants::error_badrepeat);
//     ExpectRegexError("a+{2}", boost::regex_constants::error_badrepeat);
// }

// TEST(RegexError, EscapeError) {
//     ExpectRegexError(R"(a\)", boost::regex_constants::error_escape);
//     ExpectRegexError(R"(\d)", boost::regex_constants::error_escape);
// }

// TEST(RegexCompilation, EmptyPattern) {
//     const boost::regex pattern("");
//     boost::smatch m;
//     const std::string s = "abc";

//     ASSERT_TRUE(boost::regex_search(s, m, pattern));
//     EXPECT_TRUE(m[0].str().empty());
//     EXPECT_EQ(m.position(std::size_t{0}), 0);
// }

// TEST(RegexCompilation, BasicSyntaxTreatsPlusAsLiteralCharacter) {
//     const boost::regex pattern("a+b", boost::regex::basic);

//     EXPECT_TRUE(boost::regex_match(std::string("a+b"), pattern));
//     EXPECT_FALSE(boost::regex_match(std::string("aaab"), pattern));
// }

// TEST(RegexCompilation, ExtendedSyntaxTreatsPlusAsQuantifier) {
//     const boost::regex pattern("a+b", boost::regex::extended);

//     EXPECT_TRUE(boost::regex_match(std::string("aaab"), pattern));
//     EXPECT_FALSE(boost::regex_match(std::string("a+b"), pattern));
// }