#include <boost/regex.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

int main() {
    std::string pattern_str = "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\"\367";
    std::string input_str = "T";
    boost::regex pattern(pattern_str);
    std::cout << boost::regex_match(input_str, pattern);
    
    return 0;
}