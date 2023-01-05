#ifndef PS_NEAREST_NEIGHBORS_INTERPOLATION_HPP
#define PS_NEAREST_NEIGHBORS_INTERPOLATION_HPP

#include <array>
#include <cmath>
#include <numeric>
#include <tuple>
#include <type_traits>
#include <vector>

#include <psDataScaler.hpp>
#include <psKDTree.hpp>
#include <psSmartPointer.hpp>
#include <psValueEstimator.hpp>

// Class providing nearest neighbors interpolation
template <typename NumericType, int InputDim, int OutputDim,
          typename DataScaler =
              psStandardScaler<NumericType, InputDim, InputDim + OutputDim>,
          typename PointLocator = psKDTree<NumericType, InputDim + OutputDim,
                                           maskNLower<InputDim>()>>
class psNearestNeighborsInterpolation
    : public psValueEstimator<NumericType, InputDim, OutputDim, NumericType> {

  static_assert(
      std::is_base_of_v<psPointLocator<NumericType, InputDim + OutputDim,
                                       maskNLower<InputDim>()>,
                        PointLocator>);

  static_assert(std::is_base_of_v<
                psDataScaler<NumericType, InputDim, InputDim + OutputDim>,
                DataScaler>);

  using Parent =
      psValueEstimator<NumericType, InputDim, OutputDim, NumericType>;

  using typename Parent::InputType;
  using typename Parent::OutputType;

  using Parent::data;
  using Parent::dataChanged;
  using Parent::DataDim;

  int numberOfNeighbors;
  PointLocator locator;
  NumericType distanceExponent;

public:
  psNearestNeighborsInterpolation()
      : numberOfNeighbors(3), distanceExponent(2.) {}

  psNearestNeighborsInterpolation(int passedNumberOfNeighbors,
                                  NumericType passedDistanceExponent)
      : numberOfNeighbors(passedNumberOfNeighbors),
        distanceExponent(passedDistanceExponent) {}

  void setNumberOfNeighbors(int passedNumberOfNeighbors) {
    numberOfNeighbors = passedNumberOfNeighbors;
  }

  void setDistanceExponent(NumericType passedDistanceExponent) {
    distanceExponent = passedDistanceExponent;
  }

  bool initialize() override {
    if (!data) {
      std::cout << "No data provided to psNearestNeighborsInterpolation!\n";
      return false;
    }

    if (data->size() == 0)
      return false;

    DataScaler scaler;
    scaler.setData(data);
    scaler.apply();
    auto scalingFactors = scaler.getScalingFactors();

    locator.setPoints(*data);
    locator.setScalingFactors(scalingFactors);
    locator.build();

    dataChanged = false;

    return true;
  }

  std::tuple<OutputType, NumericType>
  estimate(const InputType &input) override {
    if (dataChanged)
      if (!initialize())
        return {{}, {}};

    auto neighbors = locator.findKNearest(input, numberOfNeighbors);
    OutputType result{0};

    NumericType weightSum{0};
    NumericType minDistance{0};

    for (int j = 0; j < numberOfNeighbors; ++j) {
      auto [nearestIndex, distance] = neighbors->at(j);

      minDistance = std::min({distance, minDistance});

      NumericType weight;
      if (distance == 0) {
        for (int i = 0; i < OutputDim; ++i)
          result[i] = data->at(nearestIndex)[InputDim + i];
        weightSum = 1.;
        break;
      } else {
        weight = std::pow(1. / distance, distanceExponent);
      }
      for (int i = 0; i < OutputDim; ++i)
        result[i] += weight * data->at(nearestIndex)[InputDim + i];

      weightSum += weight;
    }

    for (int i = 0; i < OutputDim; ++i)
      result[i] /= weightSum;

    return {result, minDistance};
  }
};

#endif