#include <GeometricDistributionModels.hpp>
#include <Geometries/psMakeTrench.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psVTKWriter.hpp>
#include <psWriteVisualizationMesh.hpp>

int main() {
  using NumericType = double;
  static constexpr int D = 2;

  const NumericType gridDelta = 0.02;
  const NumericType xExtent = 1;
  const NumericType yExtent = 1;
  const NumericType trenchWidth = 0.4;
  const NumericType trenchHeight = 0.8;
  const NumericType taperAngle = 0;
  const NumericType radius = .15;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeTrench<NumericType, D>(geometry, gridDelta, xExtent, yExtent,
                               trenchWidth, trenchHeight)
      .apply();

  // copy top layer to capture deposition
  auto depoLayer = psSmartPointer<lsDomain<NumericType, D>>::New(
      geometry->getLevelSets()->back());
  geometry->insertNextLevelSet(depoLayer);

  SphereDistribution<NumericType, D> model(radius, gridDelta);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model.getProcessModel());

  auto mesh = psSmartPointer<lsMesh<NumericType>>::New();
  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "initial.vtp").apply();

  process.apply();

  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "final.vtp").apply();

  if constexpr (D == 2)
    psWriteVisualizationMesh<NumericType, D>(geometry, "final").apply();

  return EXIT_SUCCESS;
}
