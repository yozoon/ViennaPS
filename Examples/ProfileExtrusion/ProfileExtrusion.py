import viennals3d as vls
import viennaps3d as vps


def main() -> None:
    grid_delta = 0.05
    extrusion_length = 5.0
    x_extent = 2.2
    y_extent = 2.2

    geometry = vps.psDomain()
    profile = [
        [1.0, 1.0],
        [1.0, -1.0],
        [-1.0, -1.0],
        [-1.0, 1.0],
    ]

    vps.psExtrudeProfile(geometry, grid_delta, x_extent, y_extent, profile,
                         extrusion_length).apply()
    geometry.printSurface("output.vtp")


if __name__ == "__main__":
    main()
