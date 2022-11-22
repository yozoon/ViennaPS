#ifndef PS_CSV_DATA_SOURCE_HPP
#define PS_CSV_DATA_SOURCE_HPP

#include <array>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <psCSVReader.hpp>
#include <psDataSource.hpp>
#include <psSmartPointer.hpp>
#include <psUtils.hpp>

template <typename NumericType, int D>
class psCSVDataSource : public psDataSource<NumericType, D> {
  psCSVReader<NumericType, D> reader;
  std::unordered_map<std::string, NumericType> namedParameters;
  std::vector<NumericType> positionalParameters;
  bool parametersInitialized = false;

  static void
  processPositionalParam(const std::string &input,
                         std::vector<NumericType> &positionalParameters) {
    // Positional parameter
    auto v = psUtils::safeConvert<NumericType>(input);
    if (v.has_value())
      positionalParameters.push_back(v.value());
    else {
      std::cout << "Error while converting parameter '" << input
                << "' to numeric type.\n";
    }
  }

  static void processNamedParam(
      const std::string &input,
      std::unordered_map<std::string, NumericType> &namedParameters) {
    const std::string regexPattern =
        R"rgx(^[ \t]*([0-9a-zA-Z_-]+)[ \t]*=[ \t]*([0-9eE\.\-+]+)[ \t]*$)rgx";
    const std::regex rgx(regexPattern);

    std::smatch smatch;
    if (std::regex_search(input, smatch, rgx) && smatch.size() == 3) {
      auto v = psUtils::safeConvert<NumericType>(smatch[2]);
      if (v.has_value())
        namedParameters.insert({smatch[1], v.value()});
      else {
        std::cout << "Error while converting value of parameter '" << smatch[1]
                  << "'\n";
      }
    } else {
      std::cout << "Error while parsing parameter line '" << input << "'\n";
    }
  }

  static void processParamLine(
      const std::string &line, std::vector<NumericType> &positionalParameters,
      std::unordered_map<std::string, NumericType> &namedParameters) {
    std::istringstream iss(line);
    std::string tmp;

    // Skip the '#!' characters
    char c;
    iss >> c >> c;

    // Split the string at commas
    while (std::getline(iss, tmp, ',')) {
      if (tmp.find('=') == std::string::npos) {
        processPositionalParam(tmp, positionalParameters);
      } else {
        processNamedParam(tmp, namedParameters);
      }
    }
  }

  static void processComments(
      const std::string &comments,
      std::vector<NumericType> &positionalParameters,
      std::unordered_map<std::string, NumericType> &namedParameters) {
    std::istringstream cmt(comments);
    std::string line;

    // Go over each comment line
    while (std::getline(cmt, line)) {
      // Check if the line is marked as a parameter line
      if (line.rfind("#!") == 0) {
        processParamLine(line, positionalParameters, namedParameters);
      }
    }
  }

public:
  using typename psDataSource<NumericType, D>::DataPtr;

  psCSVDataSource() {}

  psCSVDataSource(std::string passedFilename, int passedOffset = 0) {
    reader.setFilename(passedFilename);
    reader.setOffset(passedOffset);
  }

  void setFilename(std::string passedFilename) {
    reader.setFilename(passedFilename);
  }

  void setOffset(int passedOffset) { reader.setOffset(passedOffset); }

  DataPtr getAll() override {
    auto [data, comments] = reader.apply();
    if (data) {
      processComments(comments, positionalParameters, namedParameters);
      parametersInitialized = true;
    }

    return data;
  }

  std::vector<NumericType> getPositionalParameters() const override {
    if (!parametersInitialized)
      std::cout << "Parameters have not been initialized yet! Call `getAll()` "
                   "first.\n";
    return positionalParameters;
  }

  std::unordered_map<std::string, NumericType>
  getNamedParameters() const override {
    if (!parametersInitialized)
      std::cout << "Parameters have not been initialized yet! Call `getAll()` "
                   "first.\n";
    return namedParameters;
  }
};

#endif