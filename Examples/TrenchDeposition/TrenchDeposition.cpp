#include <Geometries/psMakeTrench.hpp>
#include <SimpleDeposition.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psVTKWriter.hpp>
#include <psWriteVisualizationMesh.hpp>

int main() {
  using NumericType = double;
  constexpr int D = 2;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeTrench<NumericType, D>(
      geometry, 0.02 /* grid delta */, 1.0 /*x extent*/, 1.0 /*y extent*/,
      0.4 /*trench width*/, 0.8 /*trench height*/, 0.0 /* tapering angle */)
      .apply();

  // copy top layer to capture deposition
  geometry->duplicateTopLevelSet();

  auto model = psSmartPointer<SimpleDeposition<NumericType, D>>::New(
      0.1 /*particle sticking probability*/, 1.0 /*particel source power*/);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model);
  process.setNumberOfRaysPerPoint(1000);
  process.setProcessDuration(5.0);

  geometry->printSurface("initial.vtp");

  process.apply();

  geometry->printSurface("final.vtp");

  if constexpr (D == 2)
    psWriteVisualizationMesh<NumericType, D>(geometry, "final").apply();
}
