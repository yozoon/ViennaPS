/*
  This file is used to generate the python module of viennals.
  It uses pybind11 to create the modules.

  All necessary headers are included here and the interface
  of the classes which should be exposed defined
*/

#define PYBIND11_DETAILED_ERROR_MESSAGES

// correct module name macro
#define TOKENPASTE_INTERNAL(x, y, z) x##y##z
#define TOKENPASTE(x, y, z) TOKENPASTE_INTERNAL(x, y, z)
#define STRINGIZE2(s) #s
#define STRINGIZE(s) STRINGIZE2(s)
#define VIENNAPS_MODULE_VERSION STRINGIZE(VIENNAPS_VERSION)

#include <optional>
#include <vector>

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

// all header files which define API functions
#include <psAdvectionCallback.hpp>
#include <psDomain.hpp>
#include <psGDSGeometry.hpp>
#include <psGDSReader.hpp>
#include <psProcess.hpp>
#include <psProcessModel.hpp>
#include <psSurfaceModel.hpp>

// geometries
#include <psExtrudeProfile.hpp>
#include <psMakeFin.hpp>
#include <psMakeHole.hpp>
#include <psMakePlane.hpp>
#include <psMakeTrench.hpp>

// models
#include <DirectionalEtching.hpp>
#include <GeometricDistributionModels.hpp>
#include <IsotropicProcess.hpp>
#include <PlasmaDamage.hpp>
#include <SF6O2Etching.hpp>
#include <SimpleDeposition.hpp>
#include <WetEtching.hpp>

// CellSet
#include <csDenseCellSet.hpp>

// Compact
#include <psKDTree.hpp>

// other
#include <lsDomain.hpp>
#include <rayParticle.hpp>
#include <rayReflection.hpp>
#include <rayTraceDirection.hpp>
#include <rayUtil.hpp>

// always use double for python export
typedef double T;
typedef std::vector<hrleCoordType> VectorHRLEcoord;
// get dimension from cmake define
constexpr int D = VIENNAPS_PYTHON_DIMENSION;

PYBIND11_DECLARE_HOLDER_TYPE(TemplateType, psSmartPointer<TemplateType>);
PYBIND11_MAKE_OPAQUE(std::vector<T>)
PYBIND11_MAKE_OPAQUE(std::vector<unsigned>)
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>)

// define trampoline classes for interface functions
// ALSO NEED TO ADD TRAMPOLINE CLASSES FOR CLASSES
// WHICH HOLD REFERENCES TO INTERFACE(ABSTRACT) CLASSES

// could not implement CalculateVelocities, as it is of type SmartPointer
class PypsSurfaceModel : public psSurfaceModel<T> {
  using psSurfaceModel<T>::Coverages;
  using psSurfaceModel<T>::processParams;
  using psSurfaceModel<T>::getCoverages;
  using psSurfaceModel<T>::getProcessParameters;
  typedef std::vector<T> vect_type;

public:
  void initializeCoverages(unsigned numGeometryPoints) override {
    PYBIND11_OVERLOAD(void, psSurfaceModel<T>, initializeCoverages,
                      numGeometryPoints);
  }

  void initializeProcessParameters() override {
    PYBIND11_OVERLOAD(void, psSurfaceModel<T>, initializeProcessParameters);
  }

  psSmartPointer<std::vector<T>>
  calculateVelocities(psSmartPointer<psPointData<T>> Rates,
                      const std::vector<std::array<T, 3>> &coordinates,
                      const std::vector<T> &materialIDs) override {
    PYBIND11_OVERLOAD(psSmartPointer<std::vector<T>>, psSurfaceModel<T>,
                      calculateVelocities, Rates, coordinates, materialIDs);
  }

  void updateCoverages(psSmartPointer<psPointData<T>> Rates) override {
    PYBIND11_OVERLOAD(void, psSurfaceModel<T>, updateCoverages, Rates);
  }
};

// psAdvectionCallback
class PyAdvectionCallback : public psAdvectionCallback<T, D> {
protected:
  using ClassName = psAdvectionCallback<T, D>;

public:
  using ClassName::domain;

  bool applyPreAdvect(const T processTime) override {
    PYBIND11_OVERRIDE(bool, ClassName, applyPreAdvect, processTime);
  }

  bool applyPostAdvect(const T advectionTime) override {
    PYBIND11_OVERRIDE(bool, ClassName, applyPostAdvect, advectionTime);
  }
};

// // Particle Class
// template <int D> class PyParticle : public rayParticle<PyParticle<D>, T> {
//   using ClassName = rayParticle<PyParticle<D>, T>;

// public:
//   void surfaceCollision(T rayWeight, const rayTriple<T> &rayDir,
//                         const rayTriple<T> &geomNormal,
//                         const unsigned int primID, const int materialID,
//                         rayTracingData<T> &localData,
//                         const rayTracingData<T> *globalData,
//                         rayRNG &Rng) override final {
//     PYBIND11_OVERRIDE(void, ClassName, surfaceCollision, rayWeight, rayDir,
//                       geomNormal, primID, materialID, localData, globalData,
//                       Rng);
//   }

//   std::pair<T, rayTriple<T>>
//   surfaceReflection(T rayWeight, const rayTriple<T> &rayDir,
//                     const rayTriple<T> &geomNormal, const unsigned int
//                     primID, const int materialID, const rayTracingData<T>
//                     *globalData, rayRNG &Rng) override final {
//     using Pair = std::pair<T, rayTriple<T>>;
//     PYBIND11_OVERRIDE(Pair, ClassName, surfaceReflection, rayWeight, rayDir,
//                       geomNormal, primID, materialID, globalData, Rng);
//   }

//   void initNew(rayRNG &RNG) override final {
//     PYBIND11_OVERRIDE(void, ClassName, initNew, RNG);
//   }

//   int getRequiredLocalDataSize() const override final {
//     PYBIND11_OVERRIDE(int, ClassName, getRequiredLocalDataSize);
//   }

//   T getSourceDistributionPower() const override final {
//     PYBIND11_OVERRIDE(T, ClassName, getSourceDistributionPower);
//   }

//   std::vector<std::string> getLocalDataLabels() const override final {
//     PYBIND11_OVERRIDE(std::vector<std::string>, ClassName,
//     getLocalDataLabels);
//   }
// };

// psProcessModel
// could implement only the functions that were not of type smartPointer
class PyProcessModel : public psProcessModel<T, D> {
public:
  using psProcessModel<T, D>::setProcessName;
  using psProcessModel<T, D>::getProcessName;
  using psProcessModel<T, D>::insertNextParticleType;
  using psProcessModel<T, D>::setSurfaceModel;
  using psProcessModel<T, D>::setAdvectionCallback;
  using psProcessModel<T, D>::setGeometricModel;
  using psProcessModel<T, D>::setVelocityField;
  using ParticleTypeList = std::vector<std::unique_ptr<rayAbstractParticle<T>>>;
  psSmartPointer<ParticleTypeList> particles = nullptr;
  psSmartPointer<psSurfaceModel<T>> surfaceModel = nullptr;
  psSmartPointer<psAdvectionCallback<T, D>> advectionCallback = nullptr;
  psSmartPointer<psGeometricModel<T, D>> geometricModel = nullptr;
  psSmartPointer<psVelocityField<T>> velocityField = nullptr;
  std::string processName = "default";
  using ClassName = psProcessModel<T, D>;
  //    using point_to_vec = psSmartPointer <psSurfaceModel<T>>;

  psSmartPointer<psSurfaceModel<T>> getSurfaceModel() override {

    PYBIND11_OVERLOAD(psSmartPointer<psSurfaceModel<T>>, ClassName,
                      getSurfaceModel);
  }
  //    using alt_sm_ptr = psSmartPointer<ParticleTypeList>;
  //    psSmartPointer<ParticleTypeList> getParticleTypes() override{
  //        PYBIND11_OVERLOAD(
  //                alt_sm_ptr,
  //                ClassName,
  //                getParticleTypes,
  //        );
  //    }
  psSmartPointer<psAdvectionCallback<T, D>> getAdvectionCallback() override {
    using SmartPointerAdvectionCalBack_TD =
        psSmartPointer<psAdvectionCallback<T, D>>;
    PYBIND11_OVERLOAD(SmartPointerAdvectionCalBack_TD, ClassName,
                      getAdvectionCallback, );
  }
  psSmartPointer<psGeometricModel<T, D>> getGeometricModel() override {
    using SmartPointerGeometricModel_TD =
        psSmartPointer<psGeometricModel<T, D>>;
    PYBIND11_OVERLOAD(SmartPointerGeometricModel_TD, ClassName,
                      getGeometricModel, );
  }
  psSmartPointer<psVelocityField<T>> getVelocityField() {
    PYBIND11_OVERLOAD(psSmartPointer<psVelocityField<T>>, ClassName,
                      getVelocityField, );
  }
};

// psVelocityField override functions
class PyVelocityField : public psVelocityField<T> {
  using psVelocityField<T>::psVelocityField;

public:
  T getScalarVelocity(const std::array<T, 3> &coordinate, int material,
                      const std::array<T, 3> &normalVector,
                      unsigned long pointId) override {
    PYBIND11_OVERRIDE(T, psVelocityField<T>, getScalarVelocity, coordinate,
                      material, normalVector, pointId);
  }
  // if we declare a typedef for std::array<T,3>, we will no longer get this
  // error: the compiler doesn't understand why std::array gets 2 template
  // arguments
  typedef std::array<T, 3> arrayType;
  std::array<T, 3> getVectorVelocity(const std::array<T, 3> &coordinate,
                                     int material,
                                     const std::array<T, 3> &normalVector,
                                     unsigned long pointId) override {
    PYBIND11_OVERRIDE(arrayType, // add template argument here
                      psVelocityField<T>, getVectorVelocity, coordinate,
                      material, normalVector, pointId);
  }

  T getDissipationAlpha(int direction, int material,
                        const std::array<T, 3> &centralDifferences) override {
    PYBIND11_OVERRIDE(T, psVelocityField<T>, getDissipationAlpha, direction,
                      material, centralDifferences);
  }
  // a wrapper around vector is needed in our case
  //     void setVelocities(psSmartPointer<HolderForVector> passedVelocities)
  //     override{
  //         PYBIND11_OVERRIDE(
  //                 void,
  //                 psVelocityField<T>,
  //                 setVelocities,
  //                 passedVelocities
  //         );
  //     }

  bool useTranslationField() const override {
    PYBIND11_OVERRIDE(bool, psVelocityField<T>, useTranslationField, );
  }
};

PYBIND11_MODULE(VIENNAPS_MODULE_NAME, module) {
  module.doc() =
      "ViennaPS is a header-only C++ process simulation library, which "
      "includes surface and volume representations, a ray tracer, and physical "
      "models for the simulation of microelectronic fabrication processes. The "
      "main design goals are simplicity and efficiency, tailored towards "
      "scientific simulations.";

  // set version string of python module
  module.attr("__version__") = VIENNAPS_MODULE_VERSION;

  // wrap omp_set_num_threads to control number of threads
  module.def("setNumThreads", &omp_set_num_threads);

  // psSurfaceModel
  pybind11::class_<psSurfaceModel<T>, psSmartPointer<psSurfaceModel<T>>,
                   PypsSurfaceModel>(module, "psSurfaceModel")
      .def(pybind11::init<>())
      .def("initializeCoverages", &psSurfaceModel<T>::initializeCoverages)
      .def("initializeProcessParameters",
           &psSurfaceModel<T>::initializeProcessParameters)
      .def("getCoverages", &psSurfaceModel<T>::getCoverages)
      .def("getProcessParameters", &psSurfaceModel<T>::getProcessParameters)
      .def("calculateVelocities",
           [](psSurfaceModel<double> &a,
              const lsSmartPointer<lsPointData<T>> &Rates,
              const std::vector<std::array<T, 3>> &coordinates,
              const std::vector<T> &materialIDs) {
             return a.calculateVelocities(Rates, coordinates, materialIDs);
           })
      .def("updateCoverages", &psSurfaceModel<T>::updateCoverages);

  // psVelocityField
  pybind11::class_<psVelocityField<T>, psSmartPointer<psVelocityField<T>>,
                   PyVelocityField>(module, ("psVelocityField"))
      // constructors
      .def(pybind11::init<>())
      // methods
      .def("getScalarVelocity", &psVelocityField<T>::getScalarVelocity)
      .def("getVectorVelocity", &psVelocityField<T>::getVectorVelocity)
      .def("getDissipationAlpha", &psVelocityField<T>::getDissipationAlpha)
      .def("useTranslationField", &psVelocityField<T>::useTranslationField);

  // psDomain
  pybind11::class_<psDomain<T, D>, psSmartPointer<psDomain<T, D>>>(module,
                                                                   "psDomain")
      // constructors
      .def(pybind11::init(&psSmartPointer<psDomain<T, D>>::New<>))
      .def(pybind11::init(
          &psSmartPointer<psDomain<T, D>>::New<psDomain<T, D>::lsDomainType &>))
      // methods
      .def("insertNextLevelSet", &psDomain<T, D>::insertNextLevelSet,
           pybind11::arg("levelset"), pybind11::arg("wrapLowerLevelSet") = true,
           "Insert a level set to domain.")
      .def("getLevelSets",
           [](psDomain<T, D> &d)
               -> std::optional<std::vector<psSmartPointer<lsDomain<T, D>>>> {
             auto levelsets = d.getLevelSets();
             if (levelsets)
               return *levelsets;
             return std::nullopt;
           })
      .def("generateCellSet", &psDomain<T, D>::generateCellSet,
           "Generate the cell set.")
      .def("getCellSet", &psDomain<T, D>::getCellSet, "Get the cell set.")
      .def("printSurface", &psDomain<T, D>::printSurface,
           pybind11::arg("filename"), pybind11::arg("addMaterialIds") = false,
           "Print the surface of the domain.");

  //   // Shim to instantiate the particle class
  //   pybind11::class_<PyParticle<D>, psSmartPointer<PyParticle<D>>>(module,
  //                                                                  "PyParticle")
  //       .def(pybind11::init<>())
  //       .def("surfaceCollision",
  //            [](PyParticle<D> &p, T rayWeight) {
  //              rayTracingData<T> rtData;
  //              rayTriple<T> triple;
  //              rayRNG rng;
  //              p.surfaceCollision(rayWeight, triple, triple, 0, 0, rtData,
  //                                 nullptr, rng);
  //            })
  //       .def("surfaceReflection", [](PyParticle<D> &p, T rayWeight) {
  //         rayTracingData<T> rtData;
  //         rayRNG rng;
  //         rayTriple<T> triple;
  //         p.surfaceReflection(rayWeight, triple, triple, 0, 0, nullptr, rng);
  //       });

  // psProcessModel
  // modified here as it seemes the functions were not defined, and the virtual
  // functions were not overwritten didn't implement the insertNextParticleType,
  // as I do not know what that type is and didn't implement the virtual
  // functions that are of type SmartPointer<std::vector>
  pybind11::class_<psProcessModel<T, D>, psSmartPointer<psProcessModel<T, D>>,
                   PyProcessModel>
      processModel(module, "psProcessModel");

  // constructors
  processModel
      .def(pybind11::init<>())
      // functions
      .def("setProcessName", &psProcessModel<T, D>::setProcessName)
      .def("getProcessName", &psProcessModel<T, D>::getProcessName)
      .def("getSurfaceModel", &psProcessModel<T, D>::getSurfaceModel)
      .def("getAdvectionCallback", &psProcessModel<T, D>::getAdvectionCallback)
      .def("getGeometricModel", &psProcessModel<T, D>::getGeometricModel)
      .def("getVelocityField", &psProcessModel<T, D>::getVelocityField)
      .def("setSurfaceModel",
           [](psProcessModel<T, D> &pm, psSmartPointer<psSurfaceModel<T>> &sm) {
             pm.setSurfaceModel(sm);
           })
      .def("setAdvectionCallback",
           [](psProcessModel<T, D> &pm,
              psSmartPointer<psAdvectionCallback<T, D>> &ac) {
             pm.setAdvectionCallback(ac);
           })
      .def("setGeometricModel",
           [](psProcessModel<T, D> &pm,
              psSmartPointer<psGeometricModel<T, D>> &gm) {
             pm.setGeometricModel(gm);
           })
      .def("setVelocityField", [](psProcessModel<T, D> &pm,
                                  psSmartPointer<psVelocityField<T>> &vf) {
        pm.setVelocityField(vf);
      });

  // psProcess
  pybind11::class_<psProcess<T, D>, psSmartPointer<psProcess<T, D>>>(
      module, "psProcess")
      // constructors
      .def(pybind11::init(&psSmartPointer<psProcess<T, D>>::New<>))

      // methods
      .def("setDomain", &psProcess<T, D>::setDomain, "Set the process domain.")
      .def("setProcessDuration", &psProcess<T, D>::setProcessDuration,
           "Set the process duraction.")
      .def("setSourceDirection", &psProcess<T, D>::setSourceDirection,
           "Set source direction of the process.")
      .def("setNumberOfRaysPerPoint", &psProcess<T, D>::setNumberOfRaysPerPoint,
           "Set the number of rays to traced for each particle in the process. "
           "The number is per point in the process geometry")
      .def("setMaxCoverageInitIterations",
           &psProcess<T, D>::setMaxCoverageInitIterations,
           "Set the number of iterations to initialize the coverages.")
      .def("setPrintTimeInterval", &psProcess<T, D>::setPrintTimeInterval,
           "Sets the minimum time between printing intermediate results during "
           "the process. If this is set to a non-positive value, no "
           "intermediate results are printed.")
      .def("setProcessModel",
           &psProcess<T, D>::setProcessModel<psProcessModel<T, D>>,
           "Set the process model.")
      .def("apply", &psProcess<T, D>::apply, "Run the process.")
      .def("setIntegrationScheme", &psProcess<T, D>::setIntegrationScheme);

  // psAdvectionCallback
  pybind11::class_<psAdvectionCallback<T, D>,
                   psSmartPointer<psAdvectionCallback<T, D>>,
                   PyAdvectionCallback>(module, "psAdvectionCallback")
      // constructors
      .def(pybind11::init<>())
      // functions
      .def("applyPreAdvect", &psAdvectionCallback<T, D>::applyPreAdvect)
      .def("applyPostAdvect", &psAdvectionCallback<T, D>::applyPostAdvect)
      .def_readwrite("domain", &PyAdvectionCallback::domain);

  // csDenseCellSet
  pybind11::class_<csDenseCellSet<T, D>, psSmartPointer<csDenseCellSet<T, D>>>(
      module, "csDenseCellSet")
      // constructors
      .def(pybind11::init(&psSmartPointer<csDenseCellSet<T, D>>::New<>))
      // methods
      .def("writeVTU", &csDenseCellSet<T, D>::writeVTU,
           "Write the content of the cell set to a VTU file.");

  // enums
  pybind11::enum_<rayTraceDirection>(module, "rayTraceDirection")
      .value("POS_X", rayTraceDirection::POS_X)
      .value("POS_Y", rayTraceDirection::POS_Y)
      .value("POS_Z", rayTraceDirection::POS_Z)
      .value("NEG_X", rayTraceDirection::NEG_X)
      .value("NEG_Y", rayTraceDirection::NEG_Y)
      .value("NEG_Z", rayTraceDirection::NEG_Z);

  /****************************************************************************
   *                               GEOMETRIES                                 *
   ****************************************************************************/
  // psMakePlane
  pybind11::class_<psMakePlane<T, D>, psSmartPointer<psMakePlane<T, D>>>(
      module, "psMakePlane")
      .def(pybind11::init(
               &psSmartPointer<psMakePlane<T, D>>::New<
                   psSmartPointer<psDomain<T, D>> &, const T /*GridDelta*/,
                   const T /*XExtent*/, const T /*YExtent*/, const T /*Height*/,
                   const bool /*Periodic*/>),
           pybind11::arg("psDomain"), pybind11::arg("gridDelta"),
           pybind11::arg("xExtent"), pybind11::arg("yExtent"),
           pybind11::arg("height") = 0.,
           pybind11::arg("periodicBoundary") = false)
      .def("apply", &psMakePlane<T, D>::apply, "Make plane.");

  // psMakeTrench
  pybind11::class_<psMakeTrench<T, D>, psSmartPointer<psMakeTrench<T, D>>>(
      module, "psMakeTrench")
      .def(pybind11::init(
               &psSmartPointer<psMakeTrench<T, D>>::New<
                   psSmartPointer<psDomain<T, D>> &, const T /*GridDelta*/,
                   const T /*XExtent*/, const T /*YExtent*/,
                   const T /*TrenchWidth*/, const T /*TrenchHeight*/,
                   const T /*TaperingAngle*/, const T /*BaseHeight*/,
                   const bool /*PeriodicBoundary*/, const bool /*MakeMask*/>),
           pybind11::arg("psDomain"), pybind11::arg("gridDelta"),
           pybind11::arg("xExtent"), pybind11::arg("yExtent"),
           pybind11::arg("trenchWidth"), pybind11::arg("trenchHeight"),
           pybind11::arg("taperingAngle") = 0.,
           pybind11::arg("baseHeight") = 0.,
           pybind11::arg("periodicBoundary") = false,
           pybind11::arg("makeMask") = false)
      .def("apply", &psMakeTrench<T, D>::apply, "Make trench.");

  // psMakeHole
  pybind11::class_<psMakeHole<T, D>, psSmartPointer<psMakeHole<T, D>>>(
      module, "psMakeHole")
      .def(pybind11::init(
               &psSmartPointer<psMakeHole<T, D>>::New<
                   psSmartPointer<psDomain<T, D>> &, const T /*GridDelta*/,
                   const T /*XExtent*/, const T /*YExtent*/,
                   const T /*HoleRadius*/, const T /*HoleDepth*/,
                   const T /*TaperingAngle*/, const T /*BaseHeight*/,
                   const bool /*PeriodicBoundary*/, const bool /*MakeMask*/>),
           pybind11::arg("psDomain"), pybind11::arg("gridDelta"),
           pybind11::arg("xExtent"), pybind11::arg("yExtent"),
           pybind11::arg("holeRadius"), pybind11::arg("holeDepth"),
           pybind11::arg("taperingAngle") = 0.,
           pybind11::arg("baseHeight") = 0.,
           pybind11::arg("periodicBoundary") = false,
           pybind11::arg("makeMask") = false)
      .def("apply", &psMakeHole<T, D>::apply, "Make hole.");

  // psMakeFin
  pybind11::class_<psMakeFin<T, D>, psSmartPointer<psMakeFin<T, D>>>(
      module, "psMakeFin")
      .def(pybind11::init(
               &psSmartPointer<psMakeFin<T, D>>::New<
                   psSmartPointer<psDomain<T, D>> &, const T /*GridDelta*/,
                   const T /*XExtent*/, const T /*YExtent*/,
                   const T /*FinWidth*/, const T /*FinHeight*/,
                   const T /*BaseHeight*/, const bool /*PeriodicBoundary*/,
                   const bool /*MakeMask*/>),
           pybind11::arg("psDomain"), pybind11::arg("gridDelta"),
           pybind11::arg("xExtent"), pybind11::arg("yExtent"),
           pybind11::arg("finWidth"), pybind11::arg("finHeight"),
           pybind11::arg("baseHeight") = 0.,
           pybind11::arg("periodicBoundary") = false,
           pybind11::arg("makeMask") = false)
      .def("apply", &psMakeFin<T, D>::apply, "Make fin.");

  // psExtrudeProfile
  pybind11::class_<psExtrudeProfile<T, D>,
                   psSmartPointer<psExtrudeProfile<T, D>>>(module,
                                                           "psExtrudeProfile")
      .def(pybind11::init(
               &psSmartPointer<psExtrudeProfile<T, D>>::New<
                   psSmartPointer<psDomain<T, D>> &, const T /*GridDelta*/,
                   const T /*xExtent*/, const T /*yExtent*/,
                   const std::vector<std::array<T, 2>> &,
                   const T /*extrusion length*/, const bool /*full extent*/,
                   const bool /*PeriodicBoundary*/>),
           pybind11::arg("psDomain"), pybind11::arg("gridDelta"),
           pybind11::arg("xExtent"), pybind11::arg("yExtent"),
           pybind11::arg("profile"), pybind11::arg("extrusionLength"),
           pybind11::arg("fullExtent") = false,
           pybind11::arg("periodicBoundary") = false)
      .def("apply", &psExtrudeProfile<T, D>::apply, "Extrude profile.");

  /****************************************************************************
   *                               MODELS                                     *
   ****************************************************************************/
  // Simple Deposition
  pybind11::class_<SimpleDeposition<T, D>,
                   psSmartPointer<SimpleDeposition<T, D>>>(
      module, "SimpleDeposition", processModel)
      .def(pybind11::init(
               &psSmartPointer<SimpleDeposition<T, D>>::New<const T, const T>),
           pybind11::arg("stickingProbability") = 0.1,
           pybind11::arg("sourceExponent") = 1.);

  // SF6O2 Etching
  pybind11::class_<SF6O2Etching<T, D>, psSmartPointer<SF6O2Etching<T, D>>>(
      module, "SF6O2Etching", processModel)
      .def(pybind11::init(&psSmartPointer<SF6O2Etching<T, D>>::New<
                          const double, const double, const double, const T,
                          const T, const int>),
           pybind11::arg("totalIonFlux"), pybind11::arg("totalEtchantFlux"),
           pybind11::arg("totalOxygenFlux"), pybind11::arg("ionEnergy") = 100.,
           pybind11::arg("oxySputterYield") = 3.,
           pybind11::arg("maskMaterial") = 0);

  // Isotropic Process
  pybind11::class_<IsotropicProcess<T, D>,
                   psSmartPointer<IsotropicProcess<T, D>>>(
      module, "IsotropicProcess", processModel)
      .def(pybind11::init(
               &psSmartPointer<IsotropicProcess<T, D>>::New<const double,
                                                            const int>),
           pybind11::arg("isotropic rate"), pybind11::arg("maskId") = 0);

  // Directional Etching
  pybind11::class_<DirectionalEtching<T, D>,
                   psSmartPointer<DirectionalEtching<T, D>>>(
      module, "DirectionalEtching", processModel)
      .def(
          pybind11::init(
              &psSmartPointer<DirectionalEtching<T, D>>::New<
                  const std::array<T, 3> &, const T, const T, const int>),
          pybind11::arg("direction"), pybind11::arg("directionalVelocity") = 1.,
          pybind11::arg("isotropicVelocity") = 0., pybind11::arg("maskId") = 0);

  // Sphere Distribution
  pybind11::class_<SphereDistribution<T, D>,
                   psSmartPointer<SphereDistribution<T, D>>>(
      module, "SphereDistribution", processModel)
      .def(pybind11::init([](const T radius, const T gridDelta,
                             psSmartPointer<lsDomain<T, D>> mask = nullptr) {
        return psSmartPointer<SphereDistribution<T, D>>::New(radius, gridDelta,
                                                             mask);
      }));

  // Box Distribution
  pybind11::class_<BoxDistribution<T, D>,
                   psSmartPointer<BoxDistribution<T, D>>>(
      module, "BoxDistribution", processModel)
      .def(
          pybind11::init([](const std::array<T, 3> &halfAxes, const T gridDelta,
                            psSmartPointer<lsDomain<T, D>> mask = nullptr) {
            return psSmartPointer<BoxDistribution<T, D>>::New(halfAxes,
                                                              gridDelta, mask);
          }));

  // Plasma Damage
  pybind11::class_<PlasmaDamage<T, D>, psSmartPointer<PlasmaDamage<T, D>>>(
      module, "PlasmaDamage", processModel)
      .def(pybind11::init(
               &psSmartPointer<PlasmaDamage<T, D>>::New<const T, const T,
                                                        const int>),
           pybind11::arg("ionEnergy") = 100.,
           pybind11::arg("meanFreePath") = 1.,
           pybind11::arg("maskMaterial") = 0);

#if VIENNAPS_PYTHON_DIMENSION > 2
  pybind11::class_<WetEtching<T, D>, psSmartPointer<WetEtching<T, D>>>(
      module, "WetEtching", processModel)
      .def(pybind11::init(&psSmartPointer<WetEtching<T, D>>::New<const int>),
           pybind11::arg("maskId") = 0);

  // GDS file parsing
  pybind11::class_<psGDSGeometry<T, D>, psSmartPointer<psGDSGeometry<T, D>>>(
      module, "psGDSGeometry")
      // constructors
      .def(pybind11::init(&psSmartPointer<psGDSGeometry<T, D>>::New<>))
      .def(pybind11::init(&psSmartPointer<psGDSGeometry<T, D>>::New<const T>),
           pybind11::arg("gridDelta"))
      // methods
      .def("setGridDelta", &psGDSGeometry<T, D>::setGridDelta,
           "Set the grid spacing.")
      .def(
          "setBoundaryConditions",
          [](psGDSGeometry<T, D> &gds,
             std::vector<typename lsDomain<T, D>::BoundaryType> &bcs) {
            if (bcs.size() == D)
              gds.setBoundaryConditions(bcs.data());
          },
          "Set the boundary conditions")
      .def("setBoundaryPadding", &psGDSGeometry<T, D>::setBoundaryPadding,
           "Set padding between the largest point of the geometry and the "
           "boundary of the domain.")
      .def("print", &psGDSGeometry<T, D>::print, "Print the geometry contents.")
      .def("layerToLevelSet", &psGDSGeometry<T, D>::layerToLevelSet,
           "Convert a layer of the GDS geometry to a level set domain.")
      .def(
          "getBounds",
          [](psGDSGeometry<T, D> &gds) -> std::array<double, 6> {
            auto b = gds.getBounds();
            std::array<double, 6> bounds;
            for (unsigned i = 0; i < 6; ++i)
              bounds[i] = b[i];
            return bounds;
          },
          "Get the bounds of the geometry.");

  pybind11::class_<psGDSReader<T, D>, psSmartPointer<psGDSReader<T, D>>>(
      module, "psGDSReader")
      // constructors
      .def(pybind11::init(&psSmartPointer<psGDSReader<T, D>>::New<>))
      .def(pybind11::init(&psSmartPointer<psGDSReader<T, D>>::New<
                          psSmartPointer<psGDSGeometry<T, D>> &, std::string>))
      // methods
      .def("setGeometry", &psGDSReader<T, D>::setGeometry,
           "Set the domain to be parsed in.")
      .def("setFileName", &psGDSReader<T, D>::setFileName,
           "Set name of the GDS file.")
      .def("apply", &psGDSReader<T, D>::apply, "Parse the GDS file.");
#endif
}
