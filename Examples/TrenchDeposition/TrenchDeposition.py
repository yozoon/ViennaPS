"""
Example demonstrating simple deposition in a trench
"""
import viennaps2d as vps


def main() -> None:
    gridDelta = 0.02
    xExtent = 1.
    yExtent = 1.
    trenchWidth = 0.4
    trenchHeight = 0.8
    taperAngle = 0.
    processTime = 5.
    stickingProbability = 0.1
    sourcePower = 1.

    geometry = vps.psDomain()
    vps.psMakeTrench(geometry, gridDelta,
                     xExtent, yExtent,
                     trenchWidth, trenchHeight,
                     taperAngle).apply()

    model = vps.SimpleDeposition(
        stickingProbability=stickingProbability, sourcePower=sourcePower)

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model.getProcessModel())
    process.setNumberOfRaysPerPoint(1000)
    process.setProcessDuration(processTime)

    geometry.printSurface("initial.vtp")

    process.apply()

    geometry.printSurface("final.vtp")


if __name__ == "__main__":
    main()
