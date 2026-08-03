#include <boost/regex.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <string_view>
#include <charconv>


struct RunResult {
    std::size_t matches = 0;
    std::size_t misses = 0;
    std::size_t complexity_errors = 0;
};

RunResult run_match(const boost::regex& pattern, const std::string& input, std::size_t iterations) {
    RunResult result;

    for (std::size_t iteration = 0; iteration < iterations; ++iteration) {
        try {
            if (boost::regex_match(input, pattern)) {
                ++result.matches;
            } else {
                ++result.misses;
            }
        } catch (const std::runtime_error&) {
            ++result.complexity_errors;
        }
    }

    return result;
}

RunResult run_search(const boost::regex& pattern, const std::string& input, std::size_t iterations) {
    RunResult result;

    for (std::size_t iteration = 0;iteration < iterations; ++iteration) {
        try {
            if (boost::regex_search(input, pattern)) {
                ++result.matches;
            } else {
                ++result.misses;
            }
        } catch (const std::runtime_error&) {
            ++result.complexity_errors;
        }
    }

    return result;
}

RunResult run_nested_quantifier(std::size_t input_length, std::size_t iterations) {
    const boost::regex pattern( R"(^(?:a+)+$)");

    std::string input(input_length, 'a');
    input.push_back('!');

    return run_match(pattern, input, iterations);
}

RunResult run_possessive_quantifier(std::size_t input_length, std::size_t iterations) {
    const boost::regex pattern(R"(^(?:a++)+$)");

    std::string input(input_length, 'a');
    input.push_back('!');

    return run_match(pattern, input, iterations);
}

RunResult run_ambiguous_alternation( std::size_t input_length, std::size_t iterations) {
    const boost::regex pattern(
        R"(^(?:a|aa)+$)",
        boost::regex::perl);

    std::string input(input_length, 'a');
    input.push_back('!');

    return run_match(pattern, input, iterations);
}

RunResult run_long_search(std::size_t seg_count, std::size_t iterations) {
    const boost::regex pattern(
        R"((?:[a-z]{8}\d{4}){4}END)");

    const std::string seg = "abcdefgh1234";

    std::string input;
    input.reserve(seg.size() * seg_count + 3);

    for (std::size_t index = 0; index < seg_count; ++index) {
        input += seg;
    }

    input += "ENX";

    return run_search(pattern, input, iterations);
}

void print_results(std::string_view scenario, const RunResult& result) {
    std::cout << "Scenario: " << scenario << '\n' 
                << "Matches: " << result.matches << '\n'
                << "Misses: " << result.misses << '\n' 
                << "Complexity errors: " << result.complexity_errors << '\n';

}

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Missing arguments\n";
        return 1;
    }

    try {
        const auto input_size = static_cast<std::size_t>(std::stoull(argv[1]));
        const auto iterations = static_cast<std::size_t>(std::stoull(argv[2]));

        RunResult result;

        result = run_nested_quantifier(input_size, iterations);
        print_results("nested quantifier", result);

        result = run_possessive_quantifier(input_size, iterations);
        print_results("possessive quantifier", result);   

        result = run_ambiguous_alternation(input_size, iterations);        
        print_results("ambiguous alternation", result);

        result = run_long_search(input_size, iterations);
        print_results("long search", result);
 
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << '\n';
        return 1;
    }
}