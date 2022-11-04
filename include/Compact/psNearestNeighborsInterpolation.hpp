#ifndef PS_NEAREST_NEIGHBORS_INTERPOLATION_HPP
#define PS_NEAREST_NEIGHBORS_INTERPOLATION_HPP

#include <array>
#include <cmath>
#include <numeric>
#include <tuple>
#include <vector>

#ifdef _OPENMP
#include <omp.h>
#endif

#include <psKDTree.hpp>
#include <psSmartPointer.hpp>
#include <psValueEstimator.hpp>

template <typename NumericType, int D, int Dim> class psStandardScaler {
  static_assert(D <= Dim);

  using VectorType = std::array<NumericType, Dim>;

  psSmartPointer<std::vector<VectorType>> data = nullptr;
  std::array<NumericType, D> scalingFactors;

public:
  psStandardScaler() {}

  void setData(psSmartPointer<std::vector<VectorType>> passedData) {
    data = passedData;
  }

  void apply() {
    if (!data)
      return;

    std::vector<NumericType> mean(D, 0.);

#pragma omp parallel for schedule(dynamic) default(none) shared(data, mean)
    for (int i = 0; i < D; ++i)
      mean[i] = std::accumulate(data->begin(), data->end(), 0.,
                                [&](const auto &sum, const auto &element) {
                                  return sum + element[i];
                                });
    for (int i = 0; i < D; ++i)
      mean[i] /= data->size();

    std::vector<NumericType> stddev(D, 0.);

#pragma omp parallel for schedule(dynamic) default(none)                       \
    shared(data, mean, stddev)
    for (int i = 0; i < D; ++i)
      stddev[i] = std::accumulate(data->begin(), data->end(), 0.,
                                  [&](const auto &sum, const auto &element) {
                                    return sum + (element[i] - mean[i]) *
                                                     (element[i] - mean[i]);
                                  });

    for (int i = 0; i < D; ++i) {
      stddev[i] = std::sqrt(stddev[i] / data->size());
      if (stddev[i] > 0)
        scalingFactors[i] = 1.0 / stddev[i];
      else
        scalingFactors[i] = 1.0;

      // std::cout << mean[i] << ' ' << stddev[i] << '\n';
    }
  }

  std::array<NumericType, D> getScalingFactors() const {
    return scalingFactors;
  }
};

// Class providing simple linear interpolation on rectilinear data grids
template <typename NumericType, int InputDim, int OutputDim>
class psNearestNeighborsInterpolation
    : public psValueEstimator<NumericType, InputDim, OutputDim, NumericType> {

  using typename psValueEstimator<NumericType, InputDim, OutputDim,
                                  NumericType>::InputType;
  using typename psValueEstimator<NumericType, InputDim, OutputDim,
                                  NumericType>::OutputType;

  using psValueEstimator<NumericType, InputDim, OutputDim,
                         NumericType>::DataDim;
  using psValueEstimator<NumericType, InputDim, OutputDim,
                         NumericType>::dataSource;

  using DataPtr = typename decltype(dataSource)::element_type::DataPtr;
  using DataVector = std::vector<std::array<NumericType, DataDim>>;

  DataPtr data = nullptr;
  bool initialized = false;
  int numberOfNeighbors;

  psKDTree<NumericType, InputDim, DataDim> tree;

public:
  psNearestNeighborsInterpolation() : numberOfNeighbors(3) {}

  psNearestNeighborsInterpolation(int passedNumberOfNeighbors)
      : numberOfNeighbors(passedNumberOfNeighbors) {}

  void initialize() override {
    data = dataSource->getAll();
    if (!data)
      return;

    psStandardScaler<NumericType, InputDim, DataDim> scaler;
    scaler.setData(data);
    scaler.apply();
    auto scalingFactors = scaler.getScalingFactors();

    tree.setPoints(*data);
    tree.setScalingFactors(scalingFactors);
    tree.build();

    initialized = true;
  }

  std::tuple<OutputType, NumericType>
  estimate(const InputType &input) override {
    if (!initialized)
      initialize();

    auto neighbors = tree.findKNearest(input, numberOfNeighbors);
    OutputType result{0};

    NumericType weightSum{0};
    NumericType minDistance{0};

    for (int j = 0; j < numberOfNeighbors; ++j) {
      auto &[nearestIndex, distance] = neighbors->at(j);

      minDistance = std::min({distance, minDistance});

      NumericType weight;
      if (distance == 0) {
        for (int i = 0; i < OutputDim; ++i)
          result[i] = data->at(nearestIndex)[InputDim + i];
        weightSum = 1.;
        break;
      } else {
        weight = 1. / distance;
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