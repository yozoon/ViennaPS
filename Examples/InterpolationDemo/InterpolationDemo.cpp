/**
 * This example demonstrates how to use a CSV datasource in combination with
 * rectilinear grid interpolation. The CSV file contains data of a rectilinear
 * data grid (3 input dimensions + 1 output dimension). In this example the
 * rectilinear grid interpolation class is configured to also allow linear
 * extrapolation outside of the provided domain (by calling the constructor with
 * `true`)
 */
#include <iostream>

#include <psCSVDataSource.hpp>
#include <psCSVWriter.hpp>
#include <psNearestNeighborsInterpolation.hpp>
#include <psRectilinearGridInterpolation.hpp>

#include <psDataScaler.hpp>

int main() {
  using NumericType = double;

  static constexpr int InputDim = 3;
  static constexpr int TargetDim = 1;

  static constexpr int DataDim = InputDim + TargetDim;

  static constexpr int numSamples = 40;

  {
    auto dataSource =
        psSmartPointer<psCSVDataSource<NumericType, DataDim>>::New();
    dataSource->setFilename("griddata.csv");

    psRectilinearGridInterpolation<NumericType, InputDim, TargetDim>
        interpolation;
    interpolation.setDataSource(dataSource);
    interpolation.initialize();

    // Quick demo of the positional and named parameters feature
    std::cout << "Positional parameters: ";
    for (auto value : dataSource->getPositionalParameters()) {
      std::cout << value << ',';
    }
    std::cout << std::endl;

    std::cout << "Named parameters: ";
    for (auto [key, value] : dataSource->getNamedParameters()) {
      std::cout << key << '=' << value << ',';
    }
    std::cout << std::endl;
    // End of parameter demo

    psCSVWriter<NumericType, 4> writer(
        "grid_output.csv",
        "x, y, z, data\nData generated by trilinear interpolation "
        "on rectilinear grid");
    writer.initialize();

    for (int i = 0; i < numSamples; ++i)
      for (int j = 0; j < numSamples; ++j)
        for (int k = 0; k < numSamples; ++k) {
          std::array<NumericType, InputDim> x;
          // x[0] = 3. + i * (6. - 3.) / (numSamples - 1);
          // x[1] = -4. + j * (6. + 4.) / (numSamples - 1);
          // x[2] = -4. + k * (-1. + 4.) / (numSamples - 1);

          // Extrapolate
          x[0] = .1 + i * (.8 - .1) / (numSamples - 1);
          x[1] = -6. + j * (8. + 6.) / (numSamples - 1);
          x[2] = -6. + k * (1. + 6.) / (numSamples - 1);

          // We use structural bindings to directly unpack the tuple
          auto [value, isInside] = interpolation.estimate(x);

          writer.writeRow({x[0], x[1], x[2], value[0]});
        }
  }

  {
    auto dataSource =
        psSmartPointer<psCSVDataSource<NumericType, DataDim>>::New();
    dataSource->setFilename("scatterdata.csv");

    int numberOfNeighbors = 5;
    NumericType distanceExponent = 1. / 5.;

    psNearestNeighborsInterpolation<NumericType, InputDim, TargetDim>
        interpolation(numberOfNeighbors, distanceExponent);
    interpolation.setDataSource(dataSource);
    interpolation.initialize();

    psCSVWriter<NumericType, 4> writer(
        "nn_std_output.csv",
        "x, y, z, data\nData generated by nearest neighbors interpolation");
    writer.initialize();

    for (int i = 0; i < numSamples; ++i)
      for (int j = 0; j < numSamples; ++j)
        for (int k = 0; k < numSamples; ++k) {
          std::array<NumericType, InputDim> x;

          // Extrapolate
          x[0] = .1 + i * (.8 - .1) / (numSamples - 1);
          x[1] = -6. + j * (8. + 6.) / (numSamples - 1);
          x[2] = -6. + k * (1. + 6.) / (numSamples - 1);

          // We use structural bindings to directly unpack the tuple
          auto [value, minDistance] = interpolation.estimate(x);

          writer.writeRow({x[0], x[1], x[2], value[0]});
        }
  }

  {
    auto dataSource =
        psSmartPointer<psCSVDataSource<NumericType, DataDim>>::New();
    dataSource->setFilename("scatterdata.csv");

    int numberOfNeighbors = 5;
    NumericType distanceExponent = 1. / 5.;

    psNearestNeighborsInterpolation<
        NumericType, InputDim, TargetDim,
        psMedianDistanceScaler<NumericType, InputDim, DataDim>>
        interpolation(numberOfNeighbors, distanceExponent);

    interpolation.setDataSource(dataSource);
    interpolation.initialize();
    psCSVWriter<NumericType, 4> writer(
        "nn_median_output.csv",
        "x, y, z, data\nData generated by nearest neighbors interpolation");
    writer.initialize();

    for (int i = 0; i < numSamples; ++i)
      for (int j = 0; j < numSamples; ++j)
        for (int k = 0; k < numSamples; ++k) {
          std::array<NumericType, InputDim> x;

          // Extrapolate
          x[0] = .1 + i * (.8 - .1) / (numSamples - 1);
          x[1] = -6. + j * (8. + 6.) / (numSamples - 1);
          x[2] = -6. + k * (1. + 6.) / (numSamples - 1);

          // We use structural bindings to directly unpack the tuple
          auto [value, minDistance] = interpolation.estimate(x);

          writer.writeRow({x[0], x[1], x[2], value[0]});
        }
  }

  return EXIT_SUCCESS;
}
