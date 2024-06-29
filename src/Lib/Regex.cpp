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

std::vector<std::vector<std::string>> Regex::MatchAll(const std::string str) {
     std::sregex_iterator next(str.begin(), str.end(), regexPattern);
     std::sregex_iterator end;
     std::vector<std::vector<std::string>> result;
     while (next != end) {
         std::smatch match = *next;
         std::vector<std::string> groups;
         for (size_t i = 0; i < match.size(); ++i) {
             groups.push_back(match[i].str());
         }
         result.push_back(groups);
         next++;
     }
     return result;
 }