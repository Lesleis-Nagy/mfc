//
// Created by Lesleis Nagy on 15/06/2024.
//

#ifndef MFC_INCLUDE_LOADER_MICROMAG_HPP_
#define MFC_INCLUDE_LOADER_MICROMAG_HPP_

#include <exception>
#include <string>
#include <sstream>

#include <H5Cpp.h>

#include "aliases.hpp"
#include "model.hpp"

/**
 * Object that will be thrown on micromagnetic model file '*.mmf" loading
 * exception.
 */
class MicromagFileLoaderException : std::exception {

 public:

  /**
   * Constructor, will create a new exception object.
   * @param message the exception message.
   */
  explicit
  MicromagFileLoaderException(std::string message) :
      _message(std::move(message)) {}

  [[nodiscard]] const char *
  what() const noexcept override {

    return _message.c_str();

  }

 private:

  std::string _message;

};

/**
 * Object that will load micromagnetic model files.
 */
class MicromagFileLoader {

 public:

  /**
   * Default constructor.
   */
  MicromagFileLoader() = default;

  /**
   * Function that will read a file and produce a Model object.
   * @param file_name the name of the file.
   * @return a new model object, this object will only contain Mesh information.
   */
  static Model
  read(const std::string &file_name) {

    v_list vcl;
    tet_list til;
    sm_list sml;

    H5::H5File file(file_name, H5F_ACC_RDONLY);

    check_for_paths(file.getId());

    read_data_set("/mesh/vertices", file, vcl);
    read_data_set("/mesh/elements", file, til);
    read_data_set("/mesh/submesh", file, sml);

    return {vcl, til, sml};

  }

 private:

  /**
   * Function to read a data set with a given name in to a 'double' array.
   * @param data_set_name the name of the data set.
   * @param file a HDF5 file object handle.
   * @param data the 'double' output array that will be populated.
   */
  static void
  read_data_set(const std::string &data_set_name,
                H5::H5File &file,
                v_list &data) {

    data.clear();

    H5::DataSet data_set = file.openDataSet(data_set_name);
    H5::DataSpace data_space = data_set.getSpace();

    H5T_class_t type_class = data_set.getTypeClass();
    hsize_t rank;
    hsize_t dims[2];
    rank = data_space.getSimpleExtentDims(dims, nullptr);

    hsize_t dims_memory_space[2];
    dims_memory_space[0] = dims[0];
    dims_memory_space[1] = dims[1];
    H5::DataSpace memory_space(2, dims_memory_space);

    data.resize(dims[0]);
    data_set.read(data.data(),
                  H5::PredType::NATIVE_DOUBLE,
                  memory_space,
                  data_space);

  }

  /**
   * Function to read a data set with a given name in to an nx4 integer array.
   * @param data_set_name the name of the data set.
   * @param file a HDF5 file object handle.
   * @param data the nx4 integer output array that will be populated.
   */
  static void
  read_data_set(const std::string &data_set_name,
                H5::H5File &file,
                tet_list &data) {

    data.clear();

    H5::DataSet data_set = file.openDataSet(data_set_name);
    H5::DataSpace data_space = data_set.getSpace();

    H5T_class_t type_class = data_set.getTypeClass();
    hsize_t rank;
    hsize_t dims[2];
    rank = data_space.getSimpleExtentDims(dims, nullptr);

    hsize_t dims_memory_space[2];
    dims_memory_space[0] = dims[0];
    dims_memory_space[1] = dims[1];
    H5::DataSpace memory_space(2, dims_memory_space);

    data.resize(dims[0]);
    data_set.read(data.data(),
                  H5::PredType::NATIVE_UINT64,
                  memory_space,
                  data_space);

  }

  /**
   * Function to load values in a given data set to a list of integers.
   * @param data_set_name the name of the data set.
   * @param file the HDF5 file object handle.
   * @param data the data array that will be populated.
   */
  static void
  read_data_set(const std::string &data_set_name,
                H5::H5File &file,
                sm_list &data) {

    data.clear();

    H5::DataSet data_set = file.openDataSet(data_set_name);
    H5::DataSpace data_space = data_set.getSpace();

    H5T_class_t type_class = data_set.getTypeClass();
    hsize_t rank;
    hsize_t dims[2];
    rank = data_space.getSimpleExtentDims(dims, nullptr);

    hsize_t dims_memory_space[1];
    dims_memory_space[0] = dims[0];
    H5::DataSpace memory_space(1, dims_memory_space);

    data.resize(dims[0]);
    data_set.read(data.data(),
                  H5::PredType::NATIVE_UINT64,
                  memory_space,
                  data_space);

  }

  /**
   * Check that the data set paths
   * - `/mesh/vertices'
   * - `/mesh/elements'
   * - `/mesh/submesh'
   * are present in the input file.
   * @param id the HDF5 file id.
   */
  static void
  check_for_paths(hid_t id) {

    if (!path_exists(id, "/mesh/vertices")) {
      throw MicromagFileLoaderException("Path '/mesh/vertices' missing.");
    }

    if (!path_exists(id, "/mesh/elements")) {
      throw MicromagFileLoaderException("Path '/mesh/elements' missing.");
    }

    if (!path_exists(id, "/mesh/submesh")) {
      throw MicromagFileLoaderException("Path '/mesh/submesh' missing.");
    }

  }

  /**
   * Check that the given path exists in the file referenced by `id'.
   * @param id the HDF5 file id.
   * @param path the path.
   * @return true if the path exists otherwise false.
   */
  static bool
  path_exists(hid_t id, const std::string &path) {

    return H5Lexists(id, path.c_str(), H5P_DEFAULT) > 0;

  }

};

#endif //MFC_INCLUDE_LOADER_MICROMAG_HPP_
