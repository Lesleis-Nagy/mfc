//
// Created by Lesleis Nagy on 25/06/2024.
//

#ifndef MFC_INCLUDE_WRITER_MICROMAG_HPP_
#define MFC_INCLUDE_WRITER_MICROMAG_HPP_

#include <exception>
#include <string>
#include <sstream>

#include <H5Cpp.h>

#include "aliases.hpp"
#include "model.hpp"

/**
 * Object that will be thrown on micromagnetic model file `*.mmf' writing
 * exception.
 */
class MicromagFileWriterException : std::exception {

 public:

  /**
   * Constructor, will create a new exception object.
   * @param message the exception message.
   */
  explicit
  MicromagFileWriterException(std::string message) :
      _message(std::move(message)) {}

  [[nodiscard]] const char *
  what() const noexcept override {

    return _message.c_str();

  }

 private:

  std::string _message;

};

/**
 * Object that will write micromagnetic model files.
 */
class MicromagFileWriter {

 public:

  /**
   * Default constructor.
   */
  MicromagFileWriter() = default;

  /**
   * Function that will write a file.
   */
  static void
  write(const std::string &file_name, const Model &model) {

    H5::H5File file(file_name, H5F_ACC_TRUNC);

    // Write the mesh.
    write_mesh(file, model);

  }

 private:

  /**
   * Write the model's mesh to the file.
   * @param file the HDF5 file handle.
   * @param model the model.
   */
  static void
  write_mesh(H5::H5File &file, const Model &model) {

    // Create a group for the mesh.
    H5::Group grp_mesh(file.createGroup("/mesh"));

    // Write the vertices.
    write_vertices(file, model);

    // Write the elements.
    write_elements(file, model);

    // Write the submesh indices.
    write_submesh_indices(file, model);

    // Write fields.
    write_fields(file, model);

  }

  /**
   * Write the model mesh's vertices to the file.
   * @param file the HDF5 file handle.
   * @param model the model.
   */
  static void
  write_vertices(H5::H5File &file, const Model &model) {

    // Create a vertices dataset in the mesh group.
    hsize_t dim_vertices[2];
    dim_vertices[0] = model.mesh().vcl().size();
    dim_vertices[1] = 3;

    H5::DataSpace dsp_vertices(2, dim_vertices);
    H5::DataSet ds_vertices(
        file.createDataSet(
            "/mesh/vertices",
            H5::PredType::NATIVE_DOUBLE,
            dsp_vertices
        )
    );

    ds_vertices.write(model.mesh().vcl().data(), H5::PredType::NATIVE_DOUBLE);

  }

  /**
   * Write the model mesh's elements to the file.
   * @param file the HDF5 file handle.
   * @param model the model.
   */
  static void
  write_elements(H5::H5File &file, const Model &model) {

    // Create an elements dataset in the mesh group.
    hsize_t dim_elements[2];
    dim_elements[0] = model.mesh().til().size();
    dim_elements[1] = 4;

    H5::DataSpace dsp_elements(2, dim_elements);
    H5::DataSet ds_elements(
        file.createDataSet(
            "/mesh/elements",
            H5::PredType::NATIVE_UINT64,
            dsp_elements
        )
    );

    ds_elements.write(model.mesh().til().data(), H5::PredType::NATIVE_UINT64);

  }

  /**
   * Write the model mesh's submesh indices to the file.
   * @param file the HDF5 file handle.
   * @param model the model.
   */
  static void
  write_submesh_indices(H5::H5File &file, const Model &model) {

    // Create a submesh id dataset in the mesh group.
    hsize_t dim_submesh_idxs[2];
    dim_submesh_idxs[0] = model.mesh().sml().size();
    dim_submesh_idxs[1] = 1;

    H5::DataSpace dsp_submesh_idxs(1, dim_submesh_idxs);
    H5::DataSet ds_submesh_idxs(
        file.createDataSet(
            "/mesh/submesh",
            H5::PredType::NATIVE_UINT64,
            dsp_submesh_idxs
        )
    );

    ds_submesh_idxs.write(
        model.mesh().sml().data(),
        H5::PredType::NATIVE_UINT64
    );

  }

  static void
  write_fields(H5::H5File &file, const Model &model) {

    // Create a group for the fields.
    H5::Group grp_mesh(file.createGroup("/fields"));

    size_t field_idx = 0;
    for (const auto &field : model.field_list().fields()) {
      write_field(file, field, field_idx);
      field_idx++;
    }

  }

  static void
  write_field(H5::H5File &file, const Field &field, size_t id) {

    // Create a group for the field.
    std::stringstream ss_field;
    ss_field << "/fields/field" << id;
    H5::Group grp_field(file.createGroup(ss_field.str()));

    if (!field.annotation().empty()) {
      // Create the annotation attribute for the group.
      H5::StrType str_type(0, H5T_VARIABLE);
      H5::DataSpace dsp_attr(H5S_SCALAR);
      H5::Attribute att_field = grp_field.createAttribute(
          field.annotation(),
          str_type,
          dsp_attr
      );
    }

    // Create a field id dataset in the mesh group.
    hsize_t dim_field_idxs[2];
    dim_field_idxs[0] = field.vectors().size();
    dim_field_idxs[1] = 3;

    std::stringstream ss_field_vectors;
    ss_field_vectors << ss_field.str() << "/vectors";

    H5::DataSpace dsp_field_idxs(2, dim_field_idxs);
    H5::DataSet ds_field_idxs(
        file.createDataSet(
            ss_field_vectors.str(),
            H5::PredType::NATIVE_DOUBLE,
            dsp_field_idxs
        )
    );

    ds_field_idxs.write(
        field.vectors().data(),
        H5::PredType::NATIVE_DOUBLE
    );

  }

};

#endif //MFC_INCLUDE_WRITER_MICROMAG_HPP_
