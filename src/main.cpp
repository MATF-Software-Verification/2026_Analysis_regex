#include <boost/regex.hpp>
#include <iostream>
#include <chrono>

int main() {
    boost::regex pattern("(a+)+!");
    std::string input = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!";

    auto start = std::chrono::high_resolution_clock::now();

    auto match = boost::regex_match(input, pattern);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration  = end - start;

    std::cout << "Match: " << match << "\n";
    std::cout << "Time: " << duration.count() << "s\n";

    return 0;
}