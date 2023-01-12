#ifndef PS_POINT_LOCATOR_HPP
#define PS_POINT_LOCATOR_HPP

#include <array>
#include <utility> // std::pair
#include <vector>  // std::vector

#include <psSmartPointer.hpp>

template <class NumericType, int D> struct psPointLocator {
  using SizeType = std::size_t;
  using PointType = std::array<NumericType, D>;

  virtual void build() = 0;

  virtual void setPoints(const std::vector<PointType> &passedPoints) = 0;

  virtual void setScalingFactors(const PointType &passedScalingFactors) = 0;

  virtual std::pair<SizeType, NumericType>
  findNearest(const PointType &x) const = 0;

  virtual psSmartPointer<std::vector<std::pair<SizeType, NumericType>>>
  findKNearest(const PointType &x, const int k) const = 0;

  virtual psSmartPointer<std::vector<std::pair<SizeType, NumericType>>>
  findNearestWithinRadius(const PointType &x,
                          const NumericType radius) const = 0;
};
#endif