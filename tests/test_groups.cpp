#include <boost/regex.hpp>
#include <gtest/gtest.h>
#include <string>

// #### Capture Groups ####

TEST(RegexGroups, NamedGroups) {
       boost::regex re(R"((?<year>\d{4})-(?<month>\d{2})-(?<day>\d{2}))");
    std::string s = "2026-04-23";
    boost::smatch m;
    ASSERT_TRUE(boost::regex_match(s, m, re));
    EXPECT_EQ(m["year"].str(),  "2026");
    EXPECT_EQ(m["month"].str(), "04");
    EXPECT_EQ(m["day"].str(),   "23");
}

