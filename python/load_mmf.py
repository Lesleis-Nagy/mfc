import h5py
import numpy as np


def main():

    mmf = h5py.File("single.mmf", "r")

    # Process the mesh.
    mesh = mmf["mesh"]

    vertices = np.array(mesh["vertices"])
    print("Mesh vertices")
    print(vertices)

    elements = np.array(mesh["elements"])
    print("Mesh elements")
    print(elements)

    submesh_indices = np.array(mesh["submesh"])
    print("Mesh submesh indices")
    print(submesh_indices)

    fields = mmf["fields"]
    print(list(fields.keys()))

    for field_name in fields:
        field = np.array(fields[field_name]["vectors"])
        print(field)


if __name__ == "__main__":
    main()
