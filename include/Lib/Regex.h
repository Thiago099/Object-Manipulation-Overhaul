#pragma once
#include <regex>


class Regex {
    std::regex regexPattern;
public:
    Regex(std::string regex) : regexPattern(regex, std::regex_constants::icase) {}
    std::vector<std::string> Match(std::string str);
    std::vector<std::vector<std::string>> MatchAll(const std::string str);
};