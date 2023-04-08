#include <iostream>

#include <psDomain.hpp>
#include <psExtrudeProfile.hpp>
#include <psSmartPointer.hpp>

int main() {
  using NumericType = double;
  static constexpr int D = 3;

  const NumericType gridDelta = 0.05;
  const NumericType extrusionLength = 5.;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();

  std::vector<std::array<NumericType, 2>> profile;
  profile.emplace_back(std::array{1., 1.});
  profile.emplace_back(std::array{1., -1.});
  profile.emplace_back(std::array{-1., -1.});
  profile.emplace_back(std::array{-1., 1.});

  psExtrudeProfile<NumericType, D>(geometry, gridDelta, profile,
                                   extrusionLength, true)
      .apply();

  std::cout << geometry->getLevelSets()->size() << std::endl;
  geometry->printSurface("output.vtp");
}
