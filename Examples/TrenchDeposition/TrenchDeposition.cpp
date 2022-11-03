#include <Geometries/psMakeTrench.hpp>
#include <SimpleDeposition.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psVTKWriter.hpp>

int main(int argc, char *argv[]) {
  using NumericType = double;
  constexpr int D = 2;

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeTrench<NumericType, D>(geometry, 0.2 /* grid delta */, 15 /*x extent*/,
                               15 /*y extent*/, 5 /*trench width*/,
                               10 /*trench height*/, false /*create mask*/)
      .apply();

  NumericType layerThickness = 2.0;
  NumericType stickingProbability = 0.0001;
  SimpleDeposition<NumericType, D> model(
      stickingProbability /*particle sticking probability*/,
      1. /*particel source power*/);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model.getProcessModel());
  process.setNumberOfRaysPerPoint(3000);
  process.setProcessDuration(layerThickness / stickingProbability);

  auto mesh = psSmartPointer<lsMesh<NumericType>>::New();
  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "initial.vtp").apply();

  process.apply();

  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "final.vtp").apply();

  return EXIT_SUCCESS;
}
