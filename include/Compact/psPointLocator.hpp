#ifndef PS_POINT_LOCATOR_HPP
#define PS_POINT_LOCATOR_HPP

#include <array>
#include <utility> // std::pair
#include <vector>  // std::vector

#include <psSmartPointer.hpp>

template <class NumericType, int D, int Dim = D> struct psPointLocator {
  using VectorType = std::array<NumericType, Dim>;
  using PointType = std::array<NumericType, D>;
  using SizeType = std::size_t;

  psPointLocator() {}

  virtual void build() = 0;

  virtual std::pair<SizeType, NumericType>
  findNearest(const std::array<NumericType, D> &x) const = 0;

  virtual psSmartPointer<std::vector<std::pair<SizeType, NumericType>>>
  findKNearest(const std::array<NumericType, D> &x, const int k) const = 0;

  virtual psSmartPointer<std::vector<std::pair<SizeType, NumericType>>>
  findNearestWithinRadius(const std::array<NumericType, D> &x,
                          const NumericType radius) const = 0;
};
#endif