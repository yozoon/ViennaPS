#ifndef PS_UTIL_HPP
#define PS_UTIL_HPP

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>

namespace psUtils {

// Small function to print a progress bar ()
void printProgress(size_t i, size_t finalCount = 100) {
  float progress = static_cast<float>(i) / static_cast<float>(finalCount);
  int barWidth = 70;

  std::cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i) {
    if (i < pos)
      std::cout << "=";
    else if (i == pos)
      std::cout << ">";
    else
      std::cout << " ";
  }
  std::cout << "] " << static_cast<int>(progress * 100.0) << " %\r";
  std::cout.flush();
}

// Checks if a string starts with a - or not
bool isSigned(const std::string &s) {
  auto pos = s.find_first_not_of(' ');
  if (pos == s.npos)
    return false;
  if (s[pos] == '-')
    return true;
  return false;
}

// Converts string to the given numeric datatype
template <typename T> T convert(const std::string &s) {
  if constexpr (std::is_same_v<T, int>) {
    return std::stoi(s);
  } else if constexpr (std::is_same_v<T, unsigned int>) {
    if (isSigned(s))
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
    if (isSigned(s))
      throw std::invalid_argument("The value must be unsigned");
    return std::stoul(s);
  } else if constexpr (std::is_same_v<T, long long int>) {
    return std::stoll(s);
  } else if constexpr (std::is_same_v<T, unsigned long long int>) {
    if (isSigned(s))
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

// safeConvert wraps the convert function to catch exceptions. If an error
// occurs the default initialized value is returned.
template <typename T> std::optional<T> safeConvert(const std::string &s) {
  T value;
  try {
    value = convert<T>(s);
  } catch (std::exception &e) {
    std::cout << '\'' << s << "' couldn't be converted to type  '"
              << typeid(value).name() << "'\n";
    return std::nullopt;
  }
  return {value};
}

// Special conversion functions
template <typename NumericType>
inline NumericType toStrictlyPositive(const std::string &s) {
  auto value = psUtils::convert<NumericType>(s);
  if (value <= 0.0)
    throw std::invalid_argument("Value must be strictly positive.");
  return value;
};

template <typename NumericType>
inline NumericType toUnitRange(const std::string &s) {
  auto value = psUtils::convert<NumericType>(s);
  if (value > 1.0 || value <= 0.0)
    throw std::invalid_argument("Value must be in the range [1,0).");
  return value;
};

bool toBool(const std::string &s) {
  auto lower = s;
  std::transform(lower.begin(), lower.end(), lower.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  bool value = false;
  if (lower == "true" || lower == "1") {
    value = true;
  } else if (lower == "false" || lower == "0") {
    value = false;
  } else {
    throw std::invalid_argument("Failed to convert value to boolean.");
  }
  return value;
};

// Removes trailing and leading whitespace, tab characters, newline and carriage
// return chars from a string
std::string trim(const std::string &str, const std::string &avoid = " \t\n\r") {
  const auto strBegin = str.find_first_not_of(avoid);
  if (strBegin == std::string::npos)
    return std::string("");

  const auto strEnd = str.find_last_not_of(avoid);
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

template <class Iterator>
std::string join(Iterator begin, Iterator end,
                 const std::string &separator = ",") {
  std::ostringstream ostr;
  if (begin != end)
    ostr << *begin++;
  while (begin != end)
    ostr << separator << *begin++;
  return ostr.str();
}

std::unordered_map<std::string, std::string>
parseConfigStream(std::istream &input) {
  // Reads a simple config file containing a single <key>=<value> pair per line
  // and returns the content as an unordered map
  std::unordered_map<std::string, std::string> paramMap;
  std::string line;
  while (std::getline(input, line)) {
    // Remove trailing and leading whitespaces
    line = trim(line);

    // Skip this line if it is marked as a comment
    if (line.find('#') == 0 || line.empty())
      continue;

    auto splitPos = line.find('=');
    if (splitPos == std::string::npos)
      continue;

    auto keyStr = trim(line.substr(0, splitPos));
    auto valStr = trim(line.substr(splitPos + 1, line.length()));

    if (keyStr.empty() || valStr.empty())
      continue;

    paramMap.insert({keyStr, valStr});
  }
  return paramMap;
}

// Opens a file and forwards its stream to the config stream parser.
std::unordered_map<std::string, std::string>
readConfigFile(const std::string &filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cout << "Failed to open config file '" << filename << "'\n";
    return {};
  }
  return parseConfigStream(f);
}

// Class that can be used during the assigning process of a param map to the
// param struct
template <typename K, typename V, typename C = decltype(&convert<V>)>
class Item {
private:
  C conv;

public:
  K key;
  V &value;

  Item(K key_, V &value_) : key(key_), value(value_), conv(&convert<V>) {}

  Item(K key_, V &value_, C conv_) : key(key_), value(value_), conv(conv_) {}

  void operator()(const std::string &k) {
    try {
      value = conv(k);
    } catch (std::exception &e) {
      std::cout << '\'' << k << "' couldn't be converted to type of parameter '"
                << key << "'\n";
    }
  }
};

// If the key is found inthe unordered_map, then the
template <typename K, typename V, typename C>
void AssignItems(std::unordered_map<std::string, std::string> &map,
                 Item<K, V, C> &&item) {
  if (auto it = map.find(item.key); it != map.end()) {
    item(it->second);
    // Remove the item from the map, since it is now 'consumed'.
    map.erase(it);
  } else {
    std::cout << "Couldn't find '" << item.key
              << "' in parameter file. Using default value instead.\n";
  }
}

// Peels off items from parameter pack
template <typename K, typename V, typename C, typename... ARGS>
void AssignItems(std::unordered_map<std::string, std::string> &map,
                 Item<K, V, C> &&item, ARGS &&...args) {
  AssignItems(map, std::forward<Item<K, V, C>>(item));
  AssignItems(map, std::forward<ARGS>(args)...);
}

}; // namespace psUtils
#endif