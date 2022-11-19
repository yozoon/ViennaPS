#ifndef PS_UTIL_HPP
#define PS_UTIL_HPP

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace psUtils {

// Checks if a string starts with a - or not
bool is_signed(const std::string &s) {
  auto pos = s.find_first_not_of(' ');
  if (pos == s.npos)
    return false;
  if (s[pos] == '-')
    return true;
  return false;
}

// Converts string to the given numeric datatype
template <typename T> T convertToNumeric(const std::string &s) {
  if constexpr (std::is_same_v<T, int>) {
    return std::stoi(s);
  } else if constexpr (std::is_same_v<T, unsigned int>) {
    if (is_signed(s))
      throw std::invalid_argument("The value must be unsigned");
    unsigned long int val = std::stoul(s);
    unsigned int num = static_cast<unsigned int>(val);
    if (val != num)
      throw std::out_of_range("The value is larger than the largest value "
                              "representable by `unsigned int`.");
    return num;
  } else if constexpr (std::is_same_v<T, long int>) {
    return std::stol(s);
  } else if constexpr (std::is_same_v<T, unsigned long int>) {
    if (is_signed(s))
      throw std::invalid_argument("The value must be unsigned");
    return std::stoul(s);
  } else if constexpr (std::is_same_v<T, long long int>) {
    return std::stoll(s);
  } else if constexpr (std::is_same_v<T, unsigned long long int>) {
    if (is_signed(s))
      throw std::invalid_argument("The value must be unsigned");
    return std::stoull(s);
  } else if constexpr (std::is_same_v<T, float>) {
    return std::stof(s);
  } else if constexpr (std::is_same_v<T, double>) {
    return std::stod(s);
  } else if constexpr (std::is_same_v<T, long double>) {
    return std::stold(s);
  } else if constexpr (std::is_same_v<T, std::string>) {
    return s;
  } else {
    // Throws a compile time error for all types but void
    return;
  }
}

// Reads a simple config file containing a single <key>=<value> pair per line
// and returns the content as an unordered map
std::unordered_map<std::string, std::string>
readConfigFile(const std::string &filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cout << "Failed to open config file '" << filename << "'\n";
    return {};
  }

  // Regex to find trailing and leading whitespaces
  const auto wsRegex = std::regex("^ +| +$|( ) +");

  // Regular expression for extracting key and value separated by '=' as two
  // separate capture groups
  const auto keyValueRegex = std::regex(
      R"rgx([ \t]*([0-9a-zA-Z_\-\.+]+)[ \t]*=[ \t]*([0-9a-zA-Z_\-\.+]+).*$)rgx");

  std::unordered_map<std::string, std::string> paramMap;
  std::string line;
  while (std::getline(f, line)) {
    // Remove trailing and leading whitespaces
    line = std::regex_replace(line, wsRegex, "$1");
    // Skip this line if it is marked as a comment
    if (line.rfind('#') == 0)
      continue;

    // Extract key and value
    std::smatch smatch;
    if (std::regex_search(line, smatch, keyValueRegex)) {
      if (smatch.size() < 3) {
        std::cerr << "Malformed line:\n " << line;
        continue;
      }

      paramMap.insert({smatch[1], smatch[2]});
    }
  }
  return paramMap;
}
}; // namespace psUtils
#endif