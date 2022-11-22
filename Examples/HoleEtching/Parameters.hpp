#pragma once

#include <string>
#include <unordered_map>

#include <psUtils.hpp>

template <typename T> struct Parameters {
  // Domain
  T gridDelta = 0.02;
  T xExtent = 1.0;
  T yExtent = 1.0;

  // Geometry
  T holeRadius = 0.2;
  T topRadius;
  T maskHeight;
  T taperAngle;

  // Process
  T processTime;
  T totalEtchantFlux;
  T totalOxygenFlux;
  T totalIonFlux;
  T ionEnergy;
  T A_O;

  Parameters() {}

  void fromMap(const std::unordered_map<std::string, std::string> &m) {
    psUtils::AssignItems(                                    //
        m,                                                   //
        psUtils::Item{"gridDelta", gridDelta},               //
        psUtils::Item{"xExtent", xExtent},                   //
        psUtils::Item{"yExtent", yExtent},                   //
        psUtils::Item{"holeRadius", holeRadius},             //
        psUtils::Item{"topRadius", topRadius},               //
        psUtils::Item{"maskHeight", maskHeight},             //
        psUtils::Item{"taperAngle", taperAngle},             //
        psUtils::Item{"totalEtchantFlux", totalEtchantFlux}, //
        psUtils::Item{"totalOxygenFlux", totalOxygenFlux},   //
        psUtils::Item{"totalIonFlux", totalIonFlux},         //
        psUtils::Item{"ionEnergy", ionEnergy},               //
        psUtils::Item{"A_O", A_O}                            //
    );
  }
};
