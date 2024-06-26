//
// Created by Lesleis Nagy on 15/06/2024.
//

#ifndef MFC_INCLUDE_LOADER_EXODUSII_HPP_
#define MFC_INCLUDE_LOADER_EXODUSII_HPP_

#include <exception>
#include <string>
#include <sstream>

#include <H5Cpp.h>

#include "aliases.hpp"
#include "model.hpp"

/**
 * Object that will be thrown on ExodusII loading exceptions.
 */
class ExodusIILoaderException : std::exception {

 public:

  /**
   * Constructor, will create a new exception object..
   * @param message the exception message.
   */
  explicit
  ExodusIILoaderException(std::string message) :
      _message(std::move(message)) {}

  [[nodiscard]] const char *
  what() const noexcept override {

    return _message.c_str();

  }

 private:

  std::string _message;

};

/**
 * Object that will load ExodusII files.
 */
class ExodusIILoader {

 public:

  /**
   * Default constructor.
   */
  ExodusIILoader() = default;

  /**
   * Function that will read a file and produce a Model object.
   * @param file_name the name of the file.
   * @return a new model object, this object will only contain Mesh information.
   */
  static Model
  read(const std::string &file_name) {

    H5::H5File file(file_name, H5F_ACC_RDONLY);

    check_for_paths(file.getId());
    size_t nblock = 0;
    read_nblocks(file, nblock);
    check_for_paths(file.getId(), nblock);

    std::vector<double> xs;
    std::vector<double> ys;
    std::vector<double> zs;

    // Populate vcl.

    v_list vcl;

    read_data_set("/coordx", file, xs);
    read_data_set("/coordy", file, ys);
    read_data_set("/coordz", file, zs);

    // Construct vcl.
    if ((xs.size() == ys.size()) && (ys.size() == zs.size())) {
      size_t nvcl = xs.size();
      vcl.resize(nvcl);
      for (size_t i = 0; i < nvcl; ++i) {
        vcl[i] = {xs[i], ys[i], zs[i]};
      }
    } else {
      throw ExodusIILoaderException("No. of x/y/z components don't match");
    }

    // Populate til and sml;
    tet_list til;
    sm_list sml;

    for (size_t block_idx = 0; block_idx < nblock; ++block_idx) {

      // Create a string that will hold the connect info for a block.
      std::stringstream ss;
      ss << "/connect" << block_idx + 1;

      // Create a vector to hold the block that is read from HDF5.
      std::vector<std::array<int, 4>> block;
      read_data_set(ss.str(), file, block);

      for (const auto &elem : block) {
        til.push_back({(size_t) elem[0] - 1,
                       (size_t) elem[1] - 1,
                       (size_t) elem[2] - 1,
                       (size_t) elem[3] - 1});
        sml.push_back(block_idx + 1);
      }

    }

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
                std::vector<double> &data) {

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
                std::vector<std::array<int, 4>> &data) {

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
                  H5::PredType::NATIVE_INT,
                  memory_space,
                  data_space);

  }

  /**
   * Function to read the size of `/num_el_blk'.
   * @param file a HDF5 file object handle.
   * @param nblocks the number of rows in `/num_el_blk'.
   */
  static void
  read_nblocks(H5::H5File &file, size_t &nblocks) {

    H5::DataSet data_set = file.openDataSet("/num_el_blk");
    H5::DataSpace data_space = data_set.getSpace();

    hsize_t dims[2];
    size_t rank;

    rank = data_space.getSimpleExtentDims(dims, nullptr);

    nblocks = (size_t) dims[0];

  }

  /**
   * Check that the data set paths
   * - `/coordx'
   * - `/coordy'
   * - `/coordz'
   * - `/num_el_blk'
   * are present in the input file.
   * @param id the HDF5 file id.
   */
  static void
  check_for_paths(hid_t id) {

    if (!path_exists(id, "/coordx")) {
      throw ExodusIILoaderException("The path '/coordx' is missing.");
    }

    if (!path_exists(id, "/coordy")) {
      throw ExodusIILoaderException("The path '/coordy' is missing.");
    }

    if (!path_exists(id, "/coordz")) {
      throw ExodusIILoaderException("The path '/coordz' is missing.");
    }

    if (!path_exists(id, "/num_el_blk")) {
      throw ExodusIILoaderException("The path '/num_el_blk' is missing.");
    }

  }

  /**
   * Check that the data set path
   * - `/connect<nblock>'
   * exists, where `nblock` is an integer.
   * @param id the HDF5 file id.
   * @param nblock the integer associated with data set called
   *               `/connect<nblock>'.
   */
  static void
  check_for_paths(hid_t id, size_t nblock) {

    for (size_t block_idx = 0; block_idx < nblock; ++block_idx) {

      std::stringstream ss;
      ss << "/connect" << block_idx + 1;

      if (!path_exists(id, ss.str())) {
        std::stringstream ss_emsg;
        ss_emsg << "The path '/connect" << block_idx + 1 << "' is missing.";
        throw ExodusIILoaderException(ss_emsg.str());
      }

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

#endif //MFC_INCLUDE_LOADER_EXODUSII_HPP_
