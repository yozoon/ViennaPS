#ifndef PS_VOLUME_MODEL
#define PS_VOLUME_MODEL

#include <ViennaPS/DenseCellSet/csTracing.hpp>
#include <ViennaPS/psDomain.hpp>
#include <ViennaPS/psSmartPointer.hpp>

template <typename NumericType, int D> class psVolumeModel {
protected:
  psSmartPointer<psDomain<NumericType, D>> domain;
  csTracing<NumericType, D> tracer;

public:
  void setDomain(psSmartPointer<psDomain<NumericType, D>> passedDomain) {
    domain = passedDomain;
  }

  virtual void applyPreAdvect(const NumericType processTime) {}
  virtual void applyPostAdvect(const NumericType advectionTime) {}
};

#endif