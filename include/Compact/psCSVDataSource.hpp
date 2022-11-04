#ifndef PS_CSV_DATA_SOURCE_HPP
#define PS_CSV_DATA_SOURCE_HPP

#include <array>
#include <vector>

#include <psCSVReader.hpp>
#include <psDataSource.hpp>
#include <psSmartPointer.hpp>

template <typename NumericType, int D>
class psCSVDataSource : public psDataSource<NumericType, D> {
  psCSVReader<NumericType, D> reader;

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

  DataPtr getAll() override { return reader.apply(); }
};

#endif