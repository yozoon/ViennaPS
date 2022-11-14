#include <Geometries/psMakeHole.hpp>
#include <SF6O2Etching.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psUtils.hpp>
#include <psVTKWriter.hpp>

#include "Parameters.hpp"

int main(int argc, char *argv[]) {
  using NumericType = double;
  constexpr int D = 3;

  // Parse the parameters
  int P, y;

  Parameters<NumericType> params;
  if (argc > 1) {
    auto config = psUtils::readConfigFile(argv[1]);
    if (config.empty()) {
      std::cerr << "Empty config provided" << std::endl;
      return -1;
    }
    params.fromMap(config);
  }

  auto geometry = psSmartPointer<psDomain<NumericType, D>>::New();
  psMakeHole<NumericType, D>(geometry, 0.02 /* grid delta */, 1 /*x extent*/,
                             1 /*y extent*/, 0.2 /*hole radius*/,
                             1.2 /* mask height*/, true /*create mask*/)
      .apply();

  SF6O2Etching<NumericType, D> model(
      params.totalIonFlux /*ion flux*/,
      params.totalEtchantFlux /*etchant flux*/,
      params.totalOxygenFlux /*oxygen flux*/, 100 /*min ion energy (eV)*/,
      3 /*oxy sputter yield*/, 0 /*mask material ID*/);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model.getProcessModel());
  process.setMaxCoverageInitIterations(10);
  process.setNumberOfRaysPerPoint(50);
  process.setProcessDuration(params.processTime);

  auto mesh = psSmartPointer<lsMesh<NumericType>>::New();
  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "initial.vtp").apply();

  process.apply();

  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "final.vtp").apply();

  return EXIT_SUCCESS;
}
