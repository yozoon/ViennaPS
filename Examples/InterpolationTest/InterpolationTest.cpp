#include <iostream>

#include <psCSVDataSource.hpp>
#include <psRectilinearGridInterpolator.hpp>

int main(int argc, char *argv[]) {
  using NumericType = double;
  static constexpr int D = 2;

  static constexpr int InputDim = 3;
  static constexpr int TargetDim = 1;

  static constexpr int DataDim = InputDim + TargetDim;

  std::array<NumericType, InputDim> x = {3.5, 2.2, -4.0};

  for (int i = 1; i < argc && i <= InputDim; ++i)
    x[i - 1] = std::stof(argv[i]);

  auto dataSource =
      psSmartPointer<psCSVDataSource<NumericType, DataDim>>::New();
  dataSource->setFilename("griddata.csv");
  // dataSource->setOffset(1);

  psRectilinearGridInterpolation<NumericType, InputDim, TargetDim> interpolator;
  interpolator.setDataSource(dataSource);
  interpolator.initialize();

  // We use structural bindings to directly unpack the tuple of one element
  auto [value] = interpolator.estimate(x);

  std::cout << "The interpolated value is: ";
  for (auto &v : value)
    std::cout << v << ',';
  std::cout << std::endl;

  return EXIT_SUCCESS;
}
