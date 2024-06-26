//
// Created by Lesleis Nagy on 15/06/2024.
//

#ifndef MFC_INCLUDE_ALIASES_HPP_
#define MFC_INCLUDE_ALIASES_HPP_

#include <vector>
#include <unordered_map>

/**
 * A function that will produce a hash for an array of 4 values.
 */
struct tri_hasher {

  std::size_t operator()(const std::array<size_t, 3> &arr) const {
    std::hash<size_t> size_t_hash;
    return size_t_hash(arr[0]) ^ size_t_hash(arr[1]) ^ size_t_hash(arr[2]);
  }

};

/**
 * A function that will produce a hash for an array of 2 values.
 */
struct edge_hasher {

  std::size_t operator()(const std::array<size_t, 2> &arr) const {
    std::hash<size_t> size_t_hash;
    return size_t_hash(arr[0]) ^ size_t_hash(arr[1]);
  }

};

// Vertex.
typedef std::array<double, 3> vert;

// Vertex list.
typedef std::vector<vert> v_list;

// Vertex index list.
typedef std::vector<size_t> vi_list;

// Vertex to vertices map.
typedef std::unordered_map<size_t, vi_list> v_to_vs_list;

// Edge.
typedef std::array<size_t, 2> edge;

// Edge list.
typedef std::vector<edge> edge_list;

// Edge index list.
typedef std::vector<size_t> edgei_list;

// Triangle.
typedef std::array<size_t, 3> tri;

// Triangle list.
typedef std::vector<tri> tri_list;

// Triangle index list.
typedef std::vector<size_t> trii_list;

// Tetrahedron.
typedef std::array<size_t, 4> tet;

// Tetrahedron list.
typedef std::vector<tet> tet_list;

// Tetrahedron index list.
typedef std::vector<size_t> teti_list;

// Tetrahedron to tetrahedrons map.
typedef std::unordered_map<size_t, teti_list> tet_to_tets_map;

// Sub-mesh index list.
typedef std::vector<size_t> sm_list;

// Sorted triangle to tets map.
typedef std::unordered_map<tri, teti_list, tri_hasher> stri_to_tets_map;

// Sorted edge to tets map.
typedef std::unordered_map<edge, edgei_list, edge_hasher> sedge_to_tets_map;

// Sorted triangle to triangle map.
typedef std::unordered_map<tri, tri, tri_hasher> stri_to_tri_map;

// Field vector.
typedef std::array<double, 3> fv;

// Field.
typedef std::vector<fv> fv_list;

// List of field lists
typedef std::vector<fv_list> fv_lists;

// Scalar list
typedef std::vector<double> s_list;

#endif //MFC_INCLUDE_ALIASES_HPP_
