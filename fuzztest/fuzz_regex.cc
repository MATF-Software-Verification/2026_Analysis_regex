#include <boost/regex.hpp>
#include <fuzztest/fuzztest.h>
#include <gtest/gtest.h>

void MatchPattern(std::string pattern_str, std::string input_str) {
    try {
        boost::regex pattern(pattern_str);
        boost::regex_match(input_str, pattern);
    } catch(const boost::regex_error&) {
        return;
    }
    // } catch (const std::runtime_error&) {
    //     return; // boost::regex throws this due to stack overflow
    // }
}

FUZZ_TEST(RegexFuzz, MatchPattern)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(64),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128))
    .WithSeeds({
        {R"(\d{3}-\d{4})", "123-4567"},
        {R"(^[a-zA-Z0-9_]+@[a-zA-Z0-9]+\.[a-z]{2,}$)", "user@example.com"},

    });
        // {R"((a+)+$)", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!"},
        // {R"(\d+|\d+)", "1111111111111111111111111111111111111111111111111!"} //evil regex
void MatchSelf(std::string str) {
    try {
        boost::regex pattern(str);
        boost::match_results<std::string::const_iterator> what;
        boost::regex_match(str, what, pattern, boost::match_default | boost::match_partial); //SSO-Fuzz
    } catch(const boost::regex_error&) {
        return;
    }
//     } catch(const std::runtime_error&) {
//         return;
//     }
// }
}

FUZZ_TEST(RegexFuzz, MatchSelf)
    .WithDomains(fuzztest::Arbitrary<std::string>().WithMaxSize(256));


void ParseFlags(std::string pattern_str, unsigned flag_idx) {
    static const boost::regex::flag_type flags[] = {
        boost::regex::perl,
        boost::regex::extended,
        boost::regex::basic,
        boost::regex::literal,
        boost::regex::perl | boost::regex::icase,
        boost::regex::extended | boost::regex::collate,
    };
    auto flag = flags[flag_idx % std::size(flags)];

    try {
        boost::regex pattern(pattern_str, flag);
        boost::regex_search("test input 123", pattern);
    } catch (const boost::regex_error&) {
        return;
    }
}
FUZZ_TEST(RegexFuzz, ParseFlags)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(128),
        fuzztest::InRange(0, 5) // [min, max]
    ); 

void ReplaceWithFormat(std::string pattern_str, std::string input_str, std::string format_str) {
    try {
        boost::regex pattern(pattern_str);
        boost::regex_replace(input_str, pattern, format_str);
    } catch(const boost::regex_error&) {
        return;
    }
}
FUZZ_TEST(RegexFuzz, ReplaceWithFormat)
    .WithDomains(
        fuzztest::Arbitrary<std::string>().WithMaxSize(128),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128),
        fuzztest::Arbitrary<std::string>().WithMaxSize(128)
    );

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
    );
