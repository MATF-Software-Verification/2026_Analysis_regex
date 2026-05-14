#include <boost/regex.hpp>
#include <iostream>
#include <chrono>
#include <vector>
#include <string>

int main() {
    // boost::regex pattern("(a+)+!");
    // std::string input = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!";

    // auto start = std::chrono::high_resolution_clock::now();

    // auto match = boost::regex_match(input, pattern);

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> duration  = end - start;

    // std::cout << "Match: " << match << "\n";
    // std::cout << "Time: " << duration.count() << "s\n";
    // boost::regex pattern(R"((\d+))");
    // std::string s = "abc 123 edf 456xx";
    // boost::smatch m; 
  
    // boost::regex_search(s, m, pattern);
    // std::cout << "Whole match: " << m[0].str() << "\n";
    // std::cout << "First capture group: " << m[1].str() << "\n";
    // std::cout << "Number of groups: " << m.size() << "\n";
    try {
            boost::regex re("[d-a]", boost::regex::extended);
    } catch (const boost::regex_error& error) {
            std::cout << (error.code() == boost::regex_constants::error_range) << "\n";
            std::cout << error.what() << "\n";
        }

    return 0;
}