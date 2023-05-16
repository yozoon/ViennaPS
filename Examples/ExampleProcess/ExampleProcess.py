# WARNING: This script does not work at the moment!

from typing import Any

import viennals3d as vls
import viennaps3d as vps


class Particle(vps.PyParticle):

    def __init__(self, sticking_probability: float):
        super().__init__()
        self.sticking_probability = sticking_probability

    def getRequiredLocalDataSize(self) -> int:
        return 1

    def getSourceDistributionPower(self) -> float:
        return 1.0

    def getLocalDataLabels(self) -> list[str]:
        return ["particleRate"]

    def surfaceCollision(self, rayWeight, rayDir, geomNormal, primID,
                         materialID, localData, globalData, Rng) -> None:
        localData.getVectorData(
            0)[primID] += rayWeight * self.sticking_probability

    def surfaceReflection(self, rayWeight, rayDir, geomNormal, primID,
                          materialID, localData, globalData,
                          Rng) -> tuple[float, Any]:
        return self.sticking_probability, rayDir


class VelocityField(vps.psVelocityField):

    def __init__(self):
        super().__init__()
        self.velocities = []

    def getScalarVelocity(self, coordinate, material, normalVector, pointID):
        return self.velocities[pointID]

    def setVelocities(self, passedVelocities):
        self.velocities = passedVelocities


class SurfaceModel(vps.psSurfaceModel):

    def initializeCoverages(self, num_geometry_points):
        print(num_geometry_points)

    def initializeProcessParameters(self):
        pass

    def calculateVelocities(self, rates, coordinates, materialIds):
        return rates.getScalarData("particleRate")

    def updateCoverages(self, rates):
        pass


def main() -> None:
    extent = 8
    grid_delta = 0.5
    bounds = 3 * [-extent, extent]
    boundary_conditions = [
        vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
        vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
        vls.lsBoundaryConditionEnum.INFINITE_BOUNDARY
    ]

    particle = Particle(sticking_probability=0.5)
    velocity_field = VelocityField()
    surface_model = SurfaceModel()

    plane = vls.lsDomain(bounds, boundary_conditions, grid_delta)

    # create plane
    origin = (0, 0, 0)
    plane_normal = (0, 0, 1)
    vls.lsMakeGeometry(plane, vls.lsPlane(origin, plane_normal)).apply()

    domain = vps.psDomain(plane)

    model = vps.psProcessModel()
    model.insertNextParticleType(particle)
    model.setSurfaceModel(surface_model)
    model.setVelocityField(velocity_field)
    model.setProcessName("ExampleProcess")

    process = vps.psProcess()
    process.setDomain(domain)
    process.setProcessModel(model)
    process.setSourceDirection(vps.rayTraceDirection.POS_Z)
    process.setProcessDuration(5)
    process.setMaxCoverageInitIterations(10)
    process.apply()

    domain.printSurface("ExampleProcess.vtp")


if __name__ == "__main__":
    main()
