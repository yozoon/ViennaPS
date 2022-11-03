#ifndef PS_CSV_READER_HPP
#define PS_CSV_READER_HPP

#include <array>
#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <psSmartPointer.hpp>

// Simple class for reading CSV files
template <class NumericType, int NumCols> class psCSVReader {
  std::string filename;
  int offset = 0;
  char delimiter = ',';

public:
  psCSVReader() {}
  psCSVReader(std::string passedFilename, int passedOffset = 0,
              char passedDelimiter = ',')
      : filename(passedFilename), offset(passedOffset),
        delimiter(passedDelimiter) {}

  void setFilename(std::string passedFilename) { filename = passedFilename; }

  void setOffset(int passedOffset) { offset = passedOffset; }

  void setDelimiter(char passedDelimiter) { delimiter = passedDelimiter; }

  psSmartPointer<std::vector<std::array<NumericType, NumCols>>> apply() {
    std::ifstream file(filename);
    if (file.is_open()) {
      auto data =
          psSmartPointer<std::vector<std::array<NumericType, NumCols>>>::New();
      std::string line;
      int lineCount = 0;
      // Regex to find trailing and leading whitespaces
      const auto wsRegex = std::regex("^ +| +$|( ) +");
      // Iterate over each line
      while (std::getline(file, line)) {
        ++lineCount;
        if (lineCount <= offset)
          continue;

        // Remove trailing and leading whitespaces
        line = std::regex_replace(line, wsRegex, "$1");
        // Skip this line if it is marked as a comment
        if (line.rfind('#') == 0)
          continue;

        std::istringstream iss(line);
        std::string tmp;
        std::array<NumericType, NumCols> a{0};
        int i = 0;
        while (std::getline(iss, tmp, delimiter) && i < NumCols) {
          a[i] = std::stod(tmp);
          ++i;
        }
        data->push_back(a);
      }
      file.close();
      return data;
    } else {
      return nullptr;
    }
  }
};

#endif