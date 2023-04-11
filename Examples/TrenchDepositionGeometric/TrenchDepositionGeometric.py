import viennals2d as vls
import viennaps2d as vps


def main() -> None:
    gridDelta = 0.02
    xExtent = 1
    yExtent = 1
    trenchWidth = 0.4
    trenchHeight = 0.8
    radius = 0.15

    geometry = vps.psDomain()
    vps.psMakeTrench(geometry, gridDelta, xExtent, yExtent, trenchWidth,
                     trenchHeight).apply()

    depo_layer = vls.lsDomain(geometry.getLevelSets()[-1])
    geometry.insertNextLevelSet(depo_layer, False)

    model = vps.SphereDistribution(radius=radius, gridDelta=gridDelta)

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model.getProcessModel())

    geometry.printSurface("initial.vtp")

    process.apply()

    geometry.printSurface("final.vtp")


if __name__ == "__main__":
    main()
