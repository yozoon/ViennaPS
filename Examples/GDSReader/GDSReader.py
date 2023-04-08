import viennals3d as vls
import viennaps3d as vps


def main() -> None:
    grid_delta = 0.01
    mask_file_name = "mask.gds"
    boundary_conditions = [vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
                           vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
                           vls.lsBoundaryConditionEnum.INFINITE_BOUNDARY]

    mask = vps.psGDSGeometry(grid_delta)
    mask.setBoundaryConditions(boundary_conditions)
    vps.psGDSReader(mask, mask_file_name).apply()
    bounds = mask.getBounds()

    # create plane
    origin = (0, 0, 0)
    plane_normal = (0, 0, 1)
    plane = vls.lsDomain(bounds, boundary_conditions, grid_delta)
    vls.lsMakeGeometry(plane, vls.lsPlane(origin, plane_normal)).apply()

    geometry = vps.psDomain()
    geometry.insertNextLevelSet(plane, False)

    layer0 = mask.layerToLevelSet(
        0,  # layer in GDS file
        0,  # base z position
        0.1,  # mask height
        False  # invert mask
    )
    geometry.insertNextLevelSet(layer0)

    layer1 = mask.layerToLevelSet(
        1,  # layer in GDS file
        -0.15,  # base z position
        0.45,  # mask height
        False  # invert mask
    )
    geometry.insertNextLevelSet(layer1)

    geometry.printSurface("Geometry.vtp", True)


if __name__ == "__main__":
    main()
