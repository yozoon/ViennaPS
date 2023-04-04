"""
Example demonstrating simple deposition in a trench
"""
import viennaps2d as vps

gridDelta = 0.02
xExtent = 1.
yExtent = 1.

trenchWidth = 0.4
trenchHeight = 0.8
taperAngle = 0.

processTime = 5.
stickingProbability = 0.1
sourcePower = 1.


def main() -> None:
    geometry = vps.psDomain()
    vps.psMakeTrench(geometry, gridDelta,
                     xExtent, yExtent,
                     trenchWidth, trenchHeight,
                     taperAngle).apply()

    model = vps.SimpleDeposition(stickingProbability, sourcePower)

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model.getProcessModel())
    process.setNumberOfRaysPerPoint(1000)
    process.setProcessDuration(processTime)

    geometry.printSurface("initial.vtp", False)

    process.apply()

    geometry.printSurface("final.vtp", False)


if __name__ == "__main__":
    main()
