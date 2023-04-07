#pragma once

#include <array>
#include <vector>

#include <lsDomain.hpp>
#include <lsFromSurfaceMesh.hpp>
#include <lsMesh.hpp>
#include <lsMessage.hpp>

#include <psDomain.hpp>
#include <psUtils.hpp>

#include <hrleGrid.hpp>

template <typename NumericType, int D>
std::array<NumericType, 2 * D> getBoundsFromGrid(const hrleGrid<D> &grid) {
  std::array<NumericType, 2 * D> bounds{0.};
  for (unsigned i = 0; i < D; ++i) {
    // Retrieve min and max bounds for each direction that does not have
    // infinite boundary condition
    if (grid.getBoundaryConditions(i) !=
        lsDomain<NumericType, D>::BoundaryType::INFINITE_BOUNDARY) {
      auto minIndex = grid.getMinGridPoint(i);
      auto maxIndex = grid.getMaxGridPoint(i);
      bounds[2 * i] = grid.index2Coordinate(minIndex);
      bounds[2 * i + 1] = grid.index2Coordinate(maxIndex);
    }
  }
  return bounds;
}

template <typename NumericType, int D> class psMakeExtrudedProfile {
  using LSPtrType = psSmartPointer<lsDomain<NumericType, D>>;
  using PSPtrType = psSmartPointer<psDomain<NumericType, D>>;

public:
  PSPtrType domain = nullptr;
  NumericType gridDelta = .25;
  std::vector<std::array<NumericType, 2>> profile;
  NumericType extrusionLength;
  bool fullExtent;
  bool periodicBoundary;

  static constexpr int horizontalDir = 0;
  static constexpr int extrusionDir = D - 2;
  static constexpr int verticalDir = D - 1;

  psMakeExtrudedProfile(
      PSPtrType passedDomain, const NumericType passedGridDelta,
      const std::vector<std::array<NumericType, 2>> &passedProfile,
      const NumericType passedExtrusionLength, bool passedFullExtent = false,
      bool passedPeriodicBoundary = false)
      : domain(passedDomain), gridDelta(passedGridDelta),
        profile(passedProfile), extrusionLength(passedExtrusionLength),
        fullExtent(passedFullExtent), periodicBoundary(passedPeriodicBoundary) {
  }

  void apply() {
    if (!domain) {
      lsMessage::getInstance()
          .addError("Empty domain provided. Exiting now.")
          .print();
      return;
    }

    if (profile.size() < 3) {
      lsMessage::getInstance()
          .addError("The provided profile must have at least three points.")
          .print();
      return;
    }

    if (!checkProfile()) {
      lsMessage::getInstance()
          .addError("The provided profile contains intersecting edges.")
          .print();
      return;
    }

    LSPtrType substrate = nullptr;
    NumericType xExtent, yExtent;
    if (domain->getLevelSets()->size() == 0) {
      xExtent = std::max_element(profile.begin(), profile.end(),
                                 [](auto &a, auto &b) { return a[0] < b[0]; })
                    ->at(0) +
                2 * gridDelta;

      yExtent = std::max_element(profile.begin(), profile.end(),
                                 [](auto &a, auto &b) { return a[1] < b[1]; })
                    ->at(1) +
                2 * gridDelta;

      double bounds[2 * D];
      bounds[0] = -xExtent;
      bounds[1] = xExtent;

      if constexpr (D == 3) {
        if (fullExtent) {
          bounds[2] = -yExtent / 2.;
          bounds[3] = yExtent / 2.;
        } else {
          bounds[2] = -extrusionLength / 2 - 2 * gridDelta;
          bounds[3] = extrusionLength / 2 + 2 * gridDelta;
        }
        bounds[4] = -gridDelta;
        bounds[5] = gridDelta;
      } else {
        bounds[2] = -gridDelta;
        bounds[3] = gridDelta;
      }

      typename lsDomain<NumericType, D>::BoundaryType boundaryCons[D];

      for (int i = 0; i < D - 1; i++) {
        if (periodicBoundary) {
          boundaryCons[i] =
              lsDomain<NumericType, D>::BoundaryType::PERIODIC_BOUNDARY;
        } else {
          boundaryCons[i] =
              lsDomain<NumericType, D>::BoundaryType::REFLECTIVE_BOUNDARY;
        }
      }
      boundaryCons[D - 1] =
          lsDomain<NumericType, D>::BoundaryType::INFINITE_BOUNDARY;

      substrate = LSPtrType::New(bounds, boundaryCons, gridDelta);
    } else {
      auto &grid = domain->getLevelSets()->front()->getGrid();
      auto bounds = getBoundsFromGrid<NumericType, D>(grid);
      substrate = LSPtrType::New(domain->getLevelSets()->front()->getGrid());
      xExtent = bounds[2 * horizontalDir + 1] - bounds[2 * horizontalDir];
      if constexpr (D == 3)
        yExtent = bounds[2 * verticalDir + 1] - bounds[2 * verticalDir];
    }

    domain->insertNextLevelSet(substrate);

    auto mesh = lsSmartPointer<lsMesh<>>::New();

    if constexpr (D == 2) {
      // Insert all points of the profile into the mesh
      for (auto pt : profile) {
        std::array<NumericType, 3> point;
        point[0] = pt[0];
        point[1] = pt[1];
        mesh->insertNextNode(point);
      }

      // Connect all nodes of the mesh with lines
      for (unsigned i = 0; i < mesh->nodes.size() - 1; ++i)
        mesh->lines.emplace_back(std::array<unsigned, 2>{i, i + 1});

      // Close the hull mesh
      mesh->lines.emplace_back(
          std::array<unsigned, 2>{mesh->nodes.size() - 1, 0U});
    } else {
      NumericType front = extrusionLength / 2;
      NumericType back = -extrusionLength / 2;

      if (fullExtent) {
        front = yExtent / 2;
        back = -yExtent / 2;
      }

      for (auto pt : profile) {
        std::array<NumericType, 3> point;
        point[0] = pt[0];
        point[1] = front;
        point[2] = pt[1];
        mesh->insertNextNode(point);

        point[1] = back;
        mesh->insertNextNode(point);
      }

      unsigned k = profile.size();

      std::cout << k << std::endl;

      // Triangles with two nodes at the front
      for (unsigned i = 0; i < k - 1; ++i) {
        mesh->insertNextTriangle(
            std::array<unsigned, 3>{2 * (i + 1) /* Front */, 2 * i /* Front */,
                                    2 * (i + 1) + 1 /* Back*/});
      }
      mesh->insertNextTriangle(std::array<unsigned, 3>{
          0 /* Front */, 2 * (k - 1) /* Front */, 1 /* Back */});

      // Triangles with two nodes at the back
      for (unsigned i = 0; i < k - 1; ++i) {
        mesh->insertNextTriangle(std::array<unsigned, 3>{
            2 * i + 1 /* Back */, 2 * (i + 1) + 1 /* Back*/,
            2 * i /* Front */});
      }
      mesh->insertNextTriangle(std::array<unsigned, 3>{
          2 * (k - 1) + 1 /* Back */, 1 /* Back*/, 2 * (k - 1) /* Front */});

      // Determine the center point of the cross section
      std::array<NumericType, 3> center{0.};
      std::for_each(mesh->nodes.begin(), mesh->nodes.end(),
                    [&, i = 0](const auto &n) mutable {
                      if (++i % 2 == 1) {
                        center[horizontalDir] += n[horizontalDir];
                        center[verticalDir] += n[verticalDir];
                      }
                    });
      center[horizontalDir] /= k;
      center[verticalDir] /= k;

      // Front cover
      center[extrusionDir] = front;
      unsigned frontCenterIndex = mesh->insertNextNode(center);
      for (unsigned i = 0; i < k - 1; ++i) {
        mesh->insertNextTriangle(
            std::array<unsigned, 3>{2 * i, 2 * (i + 1), frontCenterIndex});
      }
      mesh->insertNextTriangle(
          std::array<unsigned, 3>{2 * (k - 1), 0, frontCenterIndex});

      // Back cover
      center[extrusionDir] = back;
      unsigned backCenterIndex = mesh->insertNextNode(center);
      for (unsigned i = 0; i < k - 1; ++i) {
        mesh->insertNextTriangle(std::array<unsigned, 3>{
            2 * (i + 1) + 1, 2 * i + 1, backCenterIndex});
      }
      mesh->insertNextTriangle(
          std::array<unsigned, 3>{1, 2 * (k - 1) + 1, backCenterIndex});
    }

#ifndef NDEBUG
    lsVTKWriter<NumericType>(mesh, "hull_mesh.vtp").apply();
#endif

    // Now create the levelset from the mesh
    lsFromSurfaceMesh<NumericType, D>(substrate, mesh).apply();
  }

private:
  bool checkProfile() const {
    // TODO: implement algorithm which checks for intersections
    return true;
  }
};