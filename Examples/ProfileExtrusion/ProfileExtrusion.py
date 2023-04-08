import viennals3d as vls
import viennaps3d as vps

grid_delta = 0.05
extrusion_length = 5.0


def main() -> None:
    geometry = vps.psDomain()
    profile = [
        [1, 1],
        [1, -1],
        [-1, -1],
        [-1, 1],
    ]

    vps.psExtrudeProfile(geometry, grid_delta,
                         profile, extrusion_length).apply()
    geometry.printSurface("output.vtp", False)


if __name__ == "__main__":
    main()
