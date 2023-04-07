import viennals3d as vls
import viennaps3d as vps

mask_file_name = "cantilever_mask.gds"
process_time = 120   # total etch time in minutes (2 hours)
x_add = 50.  # add space to domain boundary
y_add = 50.
gridDelta = 5.  # um


def main() -> None:
    gds_mask = vps.psGDSGeometry(gridDelta)

    boundary_conditions = [vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
                           vls.lsBoundaryConditionEnum.REFLECTIVE_BOUNDARY,
                           vls.lsBoundaryConditionEnum.INFINITE_BOUNDARY]
    gds_mask.setBoundaryConditions(boundary_conditions)
    gds_mask.setBoundaryPadding(x_add, y_add)

    vps.psGDSReader(gds_mask, mask_file_name).apply()

    mask = gds_mask.layerToLevelSet(
        1,  # layer in GDS file
        0,  # base z position
        4 * gridDelta,  # mask height
        True,  # invert mask
    )

    bounds = gds_mask.getBounds()
    print(bounds)

    # create plane
    origin = (0, 0, 0)
    plane_normal = (0, 0, 1)
    plane = vls.lsDomain(bounds, boundary_conditions, gridDelta)
    vls.lsMakeGeometry(plane, vls.lsPlane(origin, plane_normal)).apply()

    geometry = vps.psDomain()
    geometry.insertNextLevelSet(mask, False)
    geometry.insertNextLevelSet(plane, False)
    geometry.printSurface("InitialGeometry.vtp", False)

    model = vps.WetEtching()

    timestep = 5.0  # minutes

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model.getProcessModel())
    process.setProcessDuration(timestep * 60)
    process.setPrintIntermediate(False)
    process.setIntegrationScheme(
        vls.lsIntegrationSchemeEnum.STENCIL_LOCAL_LAX_FRIEDRICHS_1ST_ORDER)

    t = 0
    n = 0
    while t < process_time:
        process.apply()
        geometry.printSurface(f"WetEtchingSurface_{n}.vtp", False)
        t += timestep
        n += 1

    geometry.printSurface("FinalGeometry.vtp", False)


if __name__ == "__main__":
    main()
