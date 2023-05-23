#include <SF6O2Etching.hpp>
#include <psMakeHole.hpp>
#include <psProcess.hpp>
#include <psSmartPointer.hpp>
#include <psToSurfaceMesh.hpp>
#include <psUtils.hpp>

int main() {
  using NumericType = double;
  constexpr int D = 3;

  // Domain
  NumericType gridDelta = 0.02;
  NumericType xExtent = 1.0;
  NumericType yExtent = 1.0;

  // Geometry
  NumericType holeRadius = 0.2;
  NumericType topRadius = 0.2;
  NumericType maskHeight = 0.2;
  NumericType taperAngle = 0.;

  // Process
  NumericType processTime = 150;
  NumericType totalEtchantFlux = 4.5e16;
  NumericType totalOxygenFlux = 1e18;
  NumericType totalIonFlux = 2e16;
  NumericType ionEnergy = 100;
  NumericType A_O = 3.;

  // Parse the parameters
  int P, y;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeHole<NumericType, D>(
      geometry, gridDelta /* grid delta */, xExtent /*x extent*/,
      yExtent /*y extent*/, holeRadius /*hole radius*/,
      maskHeight /* mask height*/, taperAngle /* tapering angle in degrees */,
      0 /* base height */, false /* periodic boundary */, true /*create mask*/)
      .apply();

  // use pre-defined model SF6O2 etching model
  auto model = psSmartPointer<SF6O2Etching<NumericType, D>>::New(
      totalIonFlux /*ion flux*/, totalEtchantFlux /*etchant flux*/,
      totalOxygenFlux /*oxygen flux*/, ionEnergy /*min ion energy (eV)*/,
      A_O /*oxy sputter yield*/, 0 /*mask material ID*/);

  // process setup
  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model);
  process.setMaxCoverageInitIterations(10);
  process.setNumberOfRaysPerPoint(1000);
  process.setProcessDuration(processTime);

  // print initial surface
  geometry->printSurface("initial.vtp");

  // run the process
  process.apply();

  // write collected particle meta data (ion energy distribution) to a file
  process.writeParticleDataLogs("ionEnergyDistribution.txt");

  // print final surface
  geometry->printSurface("final.vtp");
}
