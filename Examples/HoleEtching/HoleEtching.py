import viennaps3d as vps


def main() -> None:
    grid_delta = 0.02
    x_extent = 1.0
    y_extent = 1.0
    hole_radius = 0.2
    mask_height = 0.2
    taper_angle = 0.0
    total_ion_flux = 2e16
    total_etchant_flux = 4.5e16
    total_oxygen_flux = 1e18
    ion_energy = 100
    oxy_sputter_yield = 3.0
    process_time = 150

    geometry = vps.psDomain()
    vps.psMakeHole(geometry, grid_delta, x_extent, y_extent, hole_radius,
                   mask_height, taper_angle, 0.0, False, True).apply()

    model = vps.SF6O2Etching(total_ion_flux, total_etchant_flux,
                             total_oxygen_flux, ion_energy, oxy_sputter_yield,
                             0)

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model)
    process.setMaxCoverageInitIterations(10)
    process.setNumberOfRaysPerPoint(1000)
    process.setProcessDuration(process_time)

    geometry.printSurface("initial.vtp")

    process.apply()

    geometry.printSurface("final.vtp")


if __name__ == "__main__":
    main()
