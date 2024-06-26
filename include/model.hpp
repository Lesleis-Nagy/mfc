//
// Created by Lesleis Nagy on 15/06/2024.
//

#ifndef MFC_INCLUDE_MODEL_HPP_
#define MFC_INCLUDE_MODEL_HPP_

#include <utility>

#include "aliases.hpp"
#include "field.hpp"
#include "mesh.hpp"

/**
 * A model consists of a mesh and a field.
 */
class Model {

 public:

  Model(v_list vcl,
        tet_list til,
        sm_list sml) :
      _mesh{std::move(vcl), std::move(til), std::move(sml)} {}

  Model(v_list vcl,
        tet_list til,
        sm_list sml,
        FieldList field_list) :
      _mesh{std::move(vcl), std::move(til), std::move(sml)},
      _field_list{std::move(field_list)} {}

  [[nodiscard]] const Mesh &
  mesh() const { return _mesh; }

  [[nodiscard]] const FieldList &
  field_list() const { return _field_list; }

  void add_list(Field field) { _field_list.add_field(std::move(field)); }

 private:

  Mesh _mesh;

  FieldList _field_list;

};

#endif //MFC_INCLUDE_MODEL_HPP_
