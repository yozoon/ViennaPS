#pragma once

#include <string>
#include <unordered_map>

#include <psUtils.hpp>

template <typename Callback>
bool doIfFound(const std::unordered_map<std::string, std::string> &m,
               const std::string &key, Callback callback) {
  if (auto item = m.find(key); item != m.end()) {
    callback(item->second);
    return true;
  }
  return false;
}

template <typename T> struct Parameters {
  T topRadius;
  T maskHeight;
  T taperAngle;
  T processTime;
  T totalEtchantFlux;
  T totalOxygenFlux;
  T totalIonFlux;
  T A_O;
  T bottomRadius = -1;

  Parameters() {}

  void fromMap(const std::unordered_map<std::string, std::string> &m) {
    doIfFound(m, "topRadius", [&](const auto &value) {
      topRadius = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "maskHeight", [&](const auto &value) {
      maskHeight = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "taperAngle", [&](const auto &value) {
      taperAngle = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "processTime", [&](const auto &value) {
      processTime = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "totalEtchantFlux", [&](const auto &value) {
      totalEtchantFlux = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "totalOxygenFlux", [&](const auto &value) {
      totalOxygenFlux = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "totalIonFlux", [&](const auto &value) {
      totalIonFlux = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "A_O", [&](const auto &value) {
      A_O = psUtils::convertToNumeric<T>(value);
    });

    doIfFound(m, "bottomRadius", [&](const auto &value) {
      bottomRadius = psUtils::convertToNumeric<T>(value);
    });
  }
};
