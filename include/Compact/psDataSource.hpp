#ifndef PS_DATA_SOURCE_HPP
#define PS_DATA_SOURCE_HPP

#include <array>
#include <vector>

#include <psSmartPointer.hpp>

template <typename NumericType, int D> class psDataSource {
public:
  using DataPtr = psSmartPointer<std::vector<std::array<NumericType, D>>>;
  virtual DataPtr getAll() = 0;
};

#endif