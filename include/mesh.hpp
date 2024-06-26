//
// Created by Lesleis Nagy on 05/04/2024.
//

#ifndef MFC_READMESH_HPP
#define MFC_READMESH_HPP

#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <stdexcept>
#include <unordered_map>

#include "aliases.hpp"

//###########################################################################//
//# Mesh.                                                                   #//
//###########################################################################//

/**
 * An object that holds information about a mesh.
 */
class Mesh {

 public:

  /**
   * Constructor will create a new mesh.
   * @param vcl the (v)ertex (c)oordinate (l)ist.
   * @param til the (t)etrahedra (i)ndex (l)ist.
   * @param sml the (s)ub-(m)esh list.
   */
  Mesh(v_list vcl, tet_list til, sm_list sml) :
      _vcl(std::move(vcl)),
      _til(std::move(til)),
      _sml(std::move(sml))
      {}

  /**
   * Retrieve the vertex coordinate list.
   * @return the vertex coordinate list.
   */
  [[nodiscard]] const v_list &
  vcl() const {

    return _vcl;

  }

  /**
   * Retrieve the tetrahedra index list.
   * @return the tetrahedra index list.
   */
  [[nodiscard]] const tet_list &
  til() const {

    return _til;

  }

  /**
   * Retrieve the sub-mesh index list.
   * @return the sub-mesh index list.
   */
  [[nodiscard]] const sm_list &
  sml() const {

    return _sml;

  }

 private:

  // Vertex list.
  v_list _vcl;

  // Tetrahedra (index) list.
  tet_list _til;

  // Sub-mesh (index) list.
  sm_list _sml;

};

#endif //MFC_READMESH_HPP
