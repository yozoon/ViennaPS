import viennaps3d as vps


def main() -> None:
    grid_delta = 0.25
    x_extent = 15.0
    y_extent = 7.0
    fin_width = 5.0
    fin_heght = 15.0
    mean_free_path = 0.75
    ion_energy = 100.0

    geometry = vps.psDomain()
    vps.psMakeFin(geometry, grid_delta, x_extent, y_extent, fin_width,
                  fin_heght, 0., False, False).apply()

    # generate cell set with depth 5 below the lowest point of the surface
    geometry.generateCellSet(5, False)

    model = vps.PlasmaDamage(ion_energy, mean_free_path, -1)

    process = vps.psProcess()
    process.setDomain(geometry)
    process.setProcessModel(model.getProcessModel())
    process.setProcessDuration(0.)  # apply volume model only

    process.apply()

    geometry.getCellSet().writeVTU("DamageModel.vtu")


if __name__ == "__main__":
    main()
