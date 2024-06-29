#include "Lib/Regex.h"


 std::vector<std::string> Regex::Match(std::string str) {
    std::smatch match;
    std::vector<std::string> result;
    if (std::regex_match(str, match, regexPattern))
    {
        for (auto& item : match) {
            result.push_back(item.str());
        }
    }
    return result;
 }