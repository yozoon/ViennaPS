#include <Geometries/psMakeFin.hpp>
#include <PlasmaDamage.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psVTKWriter.hpp>

int main() {
  using NumericType = float;
  constexpr int D = 3;

  // Domain
  NumericType gridDelta = 0.25;
  NumericType xExtent = 15.;
  NumericType yExtent = 7.;

  // Geometry
  NumericType finWidth = 5.;
  NumericType finHeight = 15.;

  // Process
  NumericType meanFreePath = 0.75;
  NumericType ionEnergy = 100.;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeFin<NumericType, D>(geometry, gridDelta, xExtent, yExtent, finWidth,
                            finHeight, 0. /* base height*/,
                            false /*periodic boundary*/, false /*create mask*/)
      .apply();

  // generate cell set with depth 5 below the lowest point of the surface
  geometry->generateCellSet(5. /*depth*/, false /*cell set below surface*/);

  PlasmaDamage<NumericType, D> model(
      ionEnergy /* mean ion energy (eV) */,
      meanFreePath /* damage ion mean free path */,
      -1 /*mask material ID (no mask)*/);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model.getProcessModel());
  process.setProcessDuration(0); // apply only damage model

  process.apply();

  geometry->getCellSet()->writeVTU("DamageModel.vtu");

  return EXIT_SUCCESS;
}
