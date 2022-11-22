#include <GeometricUniformDeposition.hpp>
#include <Geometries/psMakeTrench.hpp>
#include <psProcess.hpp>
#include <psToSurfaceMesh.hpp>
#include <psUtils.hpp>
#include <psVTKWriter.hpp>

#include "Parameters.hpp"

int main(int argc, char *argv[]) {
  using NumericType = double;
  static constexpr int D = 2;

  // Parse the parameters
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
  psMakeTrench<NumericType, D>(geometry, params.gridDelta, params.xExtent,
                               params.yExtent, params.trenchWidth,
                               params.trenchHeight, false /*create mask*/)
      .apply();

  GeometricUniformDeposition<NumericType, D> model(params.layerThickness);

  psProcess<NumericType, D> process;
  process.setDomain(geometry);
  process.setProcessModel(model.getProcessModel());

  auto mesh = psSmartPointer<lsMesh<NumericType>>::New();
  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "initial.vtp").apply();

  process.apply();

  psToSurfaceMesh<NumericType, D>(geometry, mesh).apply();
  psVTKWriter<NumericType>(mesh, "final.vtp").apply();

  return EXIT_SUCCESS;
}
