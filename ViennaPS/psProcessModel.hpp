#ifndef PS_PROCESS_MODEL
#define PS_PROCESS_MODEL

#include <rayParticle.hpp>

#include <ViennaPS/psSmartPointer.hpp>
#include <ViennaPS/psSurfaceModel.hpp>
#include <ViennaPS/psVelocityField.hpp>
#include <ViennaPS/psVolumeModel.hpp>

template <typename NumericType, int D> class psProcessModel {
private:
  using ParticleTypeList =
      std::vector<std::unique_ptr<rayAbstractParticle<NumericType>>>;

  psSmartPointer<ParticleTypeList> particles = nullptr;
  psSmartPointer<psSurfaceModel<NumericType>> surfaceModel = nullptr;
  psSmartPointer<psVolumeModel<NumericType, D>> volumeModel = nullptr;
  psSmartPointer<psVelocityField<NumericType>> velocityField = nullptr;
  std::string processName = "default";

public:
  virtual psSmartPointer<ParticleTypeList> getParticleTypes() {
    return particles;
  }
  virtual psSmartPointer<psSurfaceModel<NumericType>> getSurfaceModel() {
    return surfaceModel;
  }
  virtual psSmartPointer<psVolumeModel<NumericType, D>> getVolumeModel() {
    return volumeModel;
  }
  virtual psSmartPointer<psVelocityField<NumericType>> getVelocityField() {
    return velocityField;
  }

  void setProcessName(std::string name) { processName = name; }

  std::string getProcessName() { return processName; }

  template <typename ParticleType>
  void insertNextParticleType(std::unique_ptr<ParticleType> &passedParticle) {
    if (particles == nullptr) {
      particles = psSmartPointer<ParticleTypeList>::New();
    }
    particles->push_back(passedParticle->clone());
  }

  template <typename SurfaceModelType>
  void setSurfaceModel(psSmartPointer<SurfaceModelType> passedSurfaceModel) {
    surfaceModel = std::dynamic_pointer_cast<psSurfaceModel<NumericType>>(
        passedSurfaceModel);
  }

  template <typename VolumeModelType>
  void setVolumeModel(psSmartPointer<VolumeModelType> passedVolumeModel) {
    volumeModel = std::dynamic_pointer_cast<psVolumeModel<NumericType, D>>(
        passedVolumeModel);
  }

  template <typename VelocityFieldType>
  void setVelocityField(psSmartPointer<VelocityFieldType> passedVelocityField) {
    velocityField = std::dynamic_pointer_cast<psVelocityField<NumericType>>(
        passedVelocityField);
  }
};

#endif